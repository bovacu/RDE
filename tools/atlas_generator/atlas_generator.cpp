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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#pragma GCC diagnostic pop
#pragma clang diagnostic pop

union rde_atlas_element {
	int i_value;
	float f_value;
	const char* s_value;
	std::vector<rde_atlas_element> v_value;
};

struct rde_atlas_config {
	std::unordered_map<const char*, rde_atlas_element> data;
};

struct rde_point {
	int x = 0;
	int y = 0;
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
	stbi_uc* _atlas_pixels = (stbi_uc*)malloc(sizeof(stbi_uc) * _max_image_size * _max_image_size * 4);
	memset(_atlas_pixels, 0, _max_image_size * _max_image_size * 4);

	char _full_name[256] = { 0 };
	snprintf(_full_name, 256, "%s.png", _atlas_name);

	// NOTE: origin is top left (0, 0), being bottom right the (_max_image_size - 1, _max_image_size - 1)

	for(size_t _i = 0; _i < _rects.size(); _i++) {
		rde_atlas_rect* _rect = &_rects[_i];		
		
		if(!_rect->was_packed) {
			printf("Warning: texture %s was not packed \n", _rect->name);
			continue;
		}

		for (int _py = 0; _py < _rect->h; _py++) {
			int _height_offset = _max_image_size * _py * 4;
			
			for(int _px = 0; _px < _rect->w * 4; _px++) {
				_atlas_pixels[(_rect->x + _rect->y * _max_image_size) * 4 + _height_offset + _px] = _rect->pixels[_px + _rect->w * _py * 4];
			}
		}
	}

	if(stbi_write_png(_full_name, _max_image_size, _max_image_size, 4, _atlas_pixels, sizeof(stbi_uc) * _max_image_size * 4) == 0) {
		free(_atlas_pixels);
		printf("Error writting atlas png \n");
		exit(-1);
	}

	free(_atlas_pixels);
}

#define QUOTED(_x) "\"" + std::string(_x) + "\": "
#define QUOTED_BRACE_NL(_x) "\"" + std::string(_x) + "\": {\n"
#define QUOTED_BRACE(_x) "\"" + std::string(_x) + "\": {"
#define QUOTED_BRACKETS(_x) "\"" + std::string(_x) + "\": ["
#define QUOTED_BRACKETS_NL(_x) "\"" + std::string(_x) + "\": [\n"

void dump_atlas_to_config_file(std::vector<rde_atlas_rect>& _rects, const char* _atlas_name) {
	std::string _data = std::string("{\n");

	for(size_t _i = 0; _i < _rects.size(); _i++) {
		rde_atlas_rect* _rect = &_rects[_i];
		_rect->name[strlen(_rect->name) - 4] = 0;
		_data += std::string("\t") + QUOTED_BRACE_NL(_rect->name);
		_data += std::string("\t\t") + QUOTED_BRACKETS("position") + std::to_string(_rect->x) + ", " + std::to_string(_rect->y) + "],\n";
		_data += std::string("\t\t") + QUOTED_BRACKETS("size") + std::to_string(_rect->w) + ", " + std::to_string(_rect->h) + "]\n";
		_data += std::string("\t") + (_i == _rects.size() - 1 ? "}\n" : "},\n");
	}

	_data += "\n}";

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

bool valueInRange(int value, int min, int max) { 
	return (value >= min) && (value <= max); 
}

bool generate_atlas_data(std::vector<rde_atlas_rect>& _rects, int _max_image_size, int _pixels_of_separation) {

	std::sort(_rects.begin(), _rects.end(), [](const rde_atlas_rect& _r_0, const rde_atlas_rect& _r_1) {
		return _r_0.h > _r_1.h;
	});

	std::vector<rde_point> _available_points;
	rde_point _origin = { 0, 0 };
	_available_points.push_back(_origin);

	std::vector<rde_atlas_rect> _rects_cpy = _rects;
	std::vector<rde_atlas_rect> _packed_rects;
	_packed_rects.reserve(_rects.size());

	while(_packed_rects.size() != _rects.size()) {

		bool _rect_found = false;

		rde_atlas_rect _selected_rect;
		rde_point _selected_point;

		for(auto& _point : _available_points) {
			for (rde_atlas_rect& _rect : _rects_cpy) {
				if ((_point.x + _rect.w) > _max_image_size || (_point.y + _rect.h) > _max_image_size) {
					continue;
				}
				
				bool _good_to_pack = true;
				for(auto& _packed_rect : _packed_rects) {
					rde_atlas_rect _rec_0 = _packed_rect;
					rde_atlas_rect _rec_1;
					_rec_1.x = _point.x;
					_rec_1.y = _point.y;
					_rec_1.w = _rect.w;
					_rec_1.h = _rect.h;

					bool _x_overlap = (_rec_1.x >= _rec_0.x && _rec_1.x <= _rec_0.x + _rec_0.w) || ((_rec_1.x + _rec_1.w >= _rec_0.x && _rec_1.x + _rec_1.w <= _rec_0.x + _rec_0.w));
					bool _y_overlap = (_rec_1.y >= _rec_0.y && _rec_1.y <= _rec_0.y + _rec_0.h) || ((_rec_1.y + _rec_1.h >= _rec_0.y && _rec_1.y + _rec_1.h <= _rec_0.y + _rec_0.h));

					bool _x_overlap_reverse = (_rec_0.x >= _rec_1.x && _rec_0.x <= _rec_1.x + _rec_1.w) || ((_rec_0.x + _rec_0.w >= _rec_1.x && _rec_0.x + _rec_0.w <= _rec_1.x + _rec_1.w));
					bool _y_overlap_reverse = (_rec_0.y >= _rec_1.y && _rec_0.y <= _rec_1.y + _rec_1.h) || ((_rec_0.y + _rec_0.h >= _rec_1.y && _rec_0.y + _rec_0.h <= _rec_1.y + _rec_1.h));
				
					bool _overlap_x = _x_overlap || _x_overlap_reverse;
					bool _overlap_y = _y_overlap || _y_overlap_reverse;

					if(_overlap_x && _overlap_y) {
						_good_to_pack = false;
						break;
					}
				}

				if(!_good_to_pack) {
					continue;
				}

				_selected_rect = _rect;
				_selected_point = _point;
				_rect_found = true;
				break;		
			}

			if(_rect_found) {
				break;
			}
		}

		if(!_rect_found) {
			return false;
		}

		_rects_cpy.erase(std::find_if(_rects_cpy.begin(), _rects_cpy.end(), [_selected_rect](const rde_atlas_rect& _r) {
		    return _selected_rect.x == _r.x && _selected_rect.y == _r.y && _selected_rect.w == _r.w && _selected_rect.h == _r.h;
		}));

		_selected_rect.x = _selected_point.x;
		_selected_rect.y = _selected_point.y;
		_selected_rect.was_packed = true;

		_packed_rects.push_back(_selected_rect);

		rde_point _bottom_left_point = { _selected_rect.x + _pixels_of_separation, _selected_rect.y + _selected_rect.h + _pixels_of_separation };
		rde_point _bottom_right_point = { _selected_rect.x + _selected_rect.w + _pixels_of_separation, _selected_rect.y + _selected_rect.h + _pixels_of_separation };
		rde_point _top_right_point = { _selected_rect.x + _selected_rect.w + _pixels_of_separation, _selected_rect.y + _pixels_of_separation };

		_available_points.erase(std::find_if(_available_points.begin(), _available_points.end(), [_selected_point](const rde_point& _p) {
			return _selected_point.x == _p.x && _selected_point.y == _p.y;
		}));

		_available_points.push_back(_bottom_left_point);
		_available_points.push_back(_bottom_right_point);
		_available_points.push_back(_top_right_point);

		std::sort(_available_points.begin(), _available_points.end(), [](const rde_point& _p_0, const rde_point& _p_1) {
			return (_p_0.y == _p_1.y) ? _p_0.x < _p_1.x : _p_0.y < _p_1.y;
		});

		for(auto& _packed_rect : _packed_rects) {
			for(int _i = _available_points.size() - 1; _i >= 0; _i--) {
				rde_point _p = _available_points[_i];
				if(_p.x > _packed_rect.x && _p.x < _packed_rect.x + _packed_rect.w &&
					_p.y > _packed_rect.y && _p.y < _packed_rect.y + _packed_rect.h) {
					_available_points.erase(_available_points.begin() + _i);
				}
			}
		}
	}

	_rects.clear();

	for(auto& _rec : _packed_rects) {
		_rects.push_back(_rec);
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
				_rect.x = 0;
				_rect.y = 0;

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
	
	generate_atlas_data(_rects, _max_image_size, _pixels_of_separation);
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