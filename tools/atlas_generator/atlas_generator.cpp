#include <stdio.h>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <string>

#if _WIN32
#include "dirent.h"
#else
#include <dirent.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

union rde_atlas_element {
	int i_value;
	float f_value;
	const char* s_value;
	std::vector<rde_atlas_element> v_value;
};

struct rde_atlas_config {
	std::unordered_map<const char*, rde_atlas_element> data;
};


struct rde_atlas_rect {
  int x, y;
  int w, h;
  bool was_packed = false;
  stbi_uc* pixels = nullptr;
  char name[256];
};

struct rde_atlas_data {
	std::vector<rde_atlas_rect> rects;
	int width_and_height;
};

void dump_atlas_to_final_texture(std::vector<rde_atlas_rect>& _rects, const char* _atlas_name, int _max_image_size) {
	(void)_rects;
	(void)_atlas_name;
	stbi_uc* _atlas_pixels = (stbi_uc*)malloc(sizeof(stbi_uc) * _max_image_size * _max_image_size * 4);
	memset(_atlas_pixels, 0, _max_image_size * _max_image_size * 4);

	char _full_name[256] = { 0 };
	snprintf(_full_name, 256, "%s.png", _atlas_name);

	// NOTE: origin is top left (0, 0), being bottom right the (_max_image_size - 1, _max_image_size - 1)

	for(size_t _i = 0; _i < _rects.size(); _i++) {

		rde_atlas_rect* _rect = &_rects[_i];
		for (int _py = 0; _py < _rect->h; _py++) {
			int _height_offset = _max_image_size * _py * 4;
			
			for(int _px = 0; _px < _rect->w * 4; _px++) {
				_atlas_pixels[(_rect->x + _rect->y * _max_image_size) * 4 + _height_offset + _px] = _rect->pixels[_px + _rect->w * _py * 4];
			}
		}
	}

	stbi_write_png(_full_name, _max_image_size, _max_image_size, 4, _atlas_pixels, sizeof(stbi_uc) * _max_image_size * 4);
}

#define QUOTED_NL(_x) "\"" + std::string(_x) + "\" [\n"
#define QUOTED(_x) "\"" + std::string(_x) + "\" ["

void dump_atlas_to_config_file(std::vector<rde_atlas_rect>& _rects, const char* _atlas_name) {
	std::string _data = QUOTED_NL(_atlas_name);

	for(size_t _i = 0; _i < _rects.size(); _i++) {
		rde_atlas_rect* _rect = &_rects[_i];
		_rect->name[strlen(_rect->name) - 4] = 0;
		_data += std::string("\t") + QUOTED_NL(_rect->name);
		_data += std::string("\t\t") + QUOTED("position") + std::to_string(_rect->x) + ", " + std::to_string(_rect->y) + "],\n";
		_data += std::string("\t\t") + QUOTED("size") + std::to_string(_rect->w) + ", " + std::to_string(_rect->h) + "]\n";
		_data += std::string("\t") + (_i == _rects.size() - 1 ? "]\n" : "],\n");
	}

	_data += "]";

	char _full_name[256] = { 0 };
	snprintf(_full_name, 256, "%s.rde_atlas", _atlas_name);
	
	#if _WIN32
	FILE* _file;
	errno_t _err;
	_err = fopen_s(&_file, _full_name, "w");
	if (_err != 0) {
		printf("Error: could not create the output file for the data \n");
		exit(-1);
	}

	fprintf(_file, "%s", _data.c_str());
	fclose(_file);
	#else
	FILE* _file = fopen(_full_name, "w");
	
	if (_file == nullptr) {
		printf("Error: could not create the output file for the data \n");
		exit(-1);
	}

	fprintf(_file, "%s", _data.c_str());
	fclose(_file);
	#endif
}

bool generate_atlas_data(std::vector<rde_atlas_rect>& _rects, int _max_image_size, int _pixels_of_separation) {
	std::sort(_rects.begin(), _rects.end(), [](const rde_atlas_rect& _r_0, const rde_atlas_rect& _r_1) {
		return _r_0.h > _r_1.h;
	});

	int x_pos = 0;
	int y_pos = 0;
	int largest_h_this_row = 0;

	for (rde_atlas_rect& _rect : _rects) {
		if ((x_pos + _rect.w) > _max_image_size) {
			y_pos += largest_h_this_row;
			x_pos = 0;
			largest_h_this_row = 0;
		}
		
		if ((y_pos + _rect.h) > _max_image_size){
			return false;
		}
		
		_rect.x = x_pos;
		_rect.y = y_pos;
		
		x_pos += _rect.w + _pixels_of_separation;
    	
		if (_rect.h > largest_h_this_row){
			largest_h_this_row = _rect.h + _pixels_of_separation;
		}
    	  
		_rect.was_packed = true;		
	}

	return true;
}

std::vector<rde_atlas_rect> load_all_textures_of_atlas(const char* _dir_path) {
	(void)_dir_path;
	std::vector<rde_atlas_rect> _rects;

	DIR* _directory = nullptr;
	if ((_directory = opendir(_dir_path)) == nullptr) {
        fprintf(stderr, "Can't open %s\n", _dir_path);
        exit(-1);
    }

    struct dirent* _entry = nullptr;
	while ((_entry = readdir(_directory)) != nullptr) {
        char _full_name[256] = { 0 };
        char _file_name[256] = { 0 };
        snprintf(_full_name, 100, "%s/%s", _dir_path, _entry->d_name);
        snprintf(_file_name, 100, "%s", _entry->d_name);

        if (_entry->d_type == DT_DIR) {
            if (strcmp(_entry->d_name, ".") != 0 && strcmp(_entry->d_name, "..") != 0) {
                return load_all_textures_of_atlas(_full_name);
            }
        } else {
        	int _width, _height, _channels;
            stbi_uc* _texture = stbi_load(_full_name, &_width, &_height, &_channels, 0);
            if(_texture != nullptr) {
            	rde_atlas_rect _rect;
            	_rect.pixels = _texture;
            	_rect.w = _width;
            	_rect.h = _height;
            	memset(_rect.name, 0, 256);
				#if _WIN32
				strcpy_s(_rect.name, _file_name);
				#else
				strcpy(_rect.name, _file_name);
				#endif
            	_rects.push_back(_rect);
            } else {
            	printf("ERROR: could not load '%s' \n", _file_name);
            }
        }
    }

    closedir(_directory);

    printf("Total textures: %zu \n", _rects.size());

	return _rects;
}

int main(int _argc, char** _argv) {
	(void)_argv;
	(void)_argc;

	if(_argc != 5) {
		printf("Error: need argument 0 = path to the directory with all the textures to be packed, argument 1 = name of the atlas, argument 2 = max image size, argument 3 = pixels of separation between textures");
		exit(-1);
	}

	const char* _dir_path = _argv[1];
	const char* _atlas_name = _argv[2];
	int _max_image_size = atoi(_argv[3]);
	int _pixels_of_separation = atoi(_argv[4]);

	if((_max_image_size & (_max_image_size - 1)) != 0) {
		printf("Error: the size of the image must be a power of 2, Ex: 32, 64, 128, 256, 512, 1024, 2048... \n");
		exit(-1);
	}

	std::vector<rde_atlas_rect> _rects = load_all_textures_of_atlas(_dir_path);
	
	bool _success = generate_atlas_data(_rects, _max_image_size, _pixels_of_separation);
	while(!_success) {
		printf("Warning: Could not fit everything in a %dx%d atlas, so trying on a %dx%d \n", _max_image_size, _max_image_size, _max_image_size << 1, _max_image_size << 1);
		_max_image_size = _max_image_size << 1;
		_success = generate_atlas_data(_rects, _max_image_size, _pixels_of_separation);
	}

	dump_atlas_to_final_texture(_rects, _atlas_name, _max_image_size);
	dump_atlas_to_config_file(_rects, _atlas_name);

	for(size_t _i = 0; _i < _rects.size(); _i++) {
		stbi_image_free(_rects[_i].pixels);
	}
}