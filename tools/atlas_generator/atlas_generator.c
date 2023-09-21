#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#if _WIN32
#include "dirent.h"
#else
#include <dirent.h>
#endif

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#define STB_DS_IMPLEMENTATION
#define STBDS_SIPHASH_2_4
#include "external/include/stb/stb_ds.h"

typedef union rde_atlas_element rde_atlas_element;

union rde_atlas_element {
	int i_value;
	float f_value;
	const char* s_value;
	rde_atlas_element* v_value;
};

typedef struct {
	int x;
	int y;
} rde_point;


typedef struct {
  int x, y;
  int w, h;
  bool was_packed;
  stbi_uc* pixels;
  char name[256];
} rde_atlas_rect;

typedef struct {
	rde_atlas_rect* rects;
	int width_and_height;
} rde_atlas_data;

void dump_atlas_to_final_texture(rde_atlas_rect* _rects, const char* _atlas_name, int _max_image_size) {
	stbi_uc* _atlas_pixels = (stbi_uc*)malloc(sizeof(stbi_uc) * _max_image_size * _max_image_size * 4);
	memset(_atlas_pixels, 0, _max_image_size * _max_image_size * 4);

	char _full_name[256] = { 0 };
	snprintf(_full_name, 256, "%s.png", _atlas_name);

	// NOTE: origin is top left (0, 0), being bottom right the (_max_image_size - 1, _max_image_size - 1)

	for(size_t _i = 0; _i < arrlen(_rects); _i++) {
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

#define CONCAT_INT(_data, _int) 	\
	do {							\
		char _str[6];				\
		sprintf(_str, "%d", _int);	\
		strcat(_data, _str);		\
	} while(0)

void dump_atlas_to_config_file(rde_atlas_rect* _rects, const char* _atlas_name) {

	size_t _data_size = 1024 * 1024;
	char* _data = (char*)malloc(sizeof(char) * _data_size);
	memset(_data, 0, _data_size);
	strcat(_data, "{\n");

	for(size_t _i = 0; _i < arrlen(_rects); _i++) {
		rde_atlas_rect* _rect = &_rects[_i];
		_rect->name[strlen(_rect->name) - 4] = 0;
		strcat(_data, "\t\""); strcat(_data, _rect->name); strcat(_data, "\": {\n");
		strcat(_data, "\t\t\""); strcat(_data, "position\": ["); CONCAT_INT(_data, _rect->x); strcat(_data, ", "); CONCAT_INT(_data, _rect->y); strcat(_data, "],\n");
		strcat(_data, "\t\t\""); strcat(_data, "size\": ["); CONCAT_INT(_data, _rect->w); strcat(_data, ", "); CONCAT_INT(_data, _rect->h); strcat(_data, "]\n");
		strcat(_data, "\t");
		if(_i == arrlen(_rects) - 1) {
			strcat(_data, "}\n");
		} else {
			strcat(_data, "},\n");
		}
	}

	strcat(_data, "\n}");

	char _full_name[256] = { 0 };
	snprintf(_full_name, 256, "%s.rde_atlas", _atlas_name);
	
	FILE* _file = fopen(_full_name, "w");
	
	if (_file == NULL) {
		printf("Error: could not create the output file for the data \n");
		exit(-1);
	}

	fprintf(_file, "%s", _data);
	fclose(_file);

	free(_data);
}

bool valueInRange(int value, int min, int max) { 
	return (value >= min) && (value <= max); 
}

int cmp_func_rects(const void* _a, const void* _b) {
	rde_atlas_rect _r_0 = *((rde_atlas_rect*)_a);
	rde_atlas_rect _r_1 = *((rde_atlas_rect*)_b);

	if(_r_0.h > _r_1.h) return -1;
	if(_r_0.h < _r_1.h) return 1;

   	return 0;
}

int cmp_func_points(const void* _a, const void* _b) {
   	rde_point _p_0 = *((rde_point*)_a);
	rde_point _p_1 = *((rde_point*)_b);

	if(_p_0.y == _p_1.y) {
		if(_p_0.x < _p_1.x) return -1;
		if(_p_0.x > _p_1.x) return 1;
		return 0;
	} else {
		if(_p_0.y < _p_1.y) return -1;
		if(_p_0.y > _p_1.y) return 1;
	}

   	return 0;
}

bool generate_atlas_data(rde_atlas_rect* _rects, int _max_image_size, int _pixels_of_separation) {

	qsort(_rects, arrlen(_rects), sizeof(rde_atlas_rect), cmp_func_rects);

	rde_point* _available_points = NULL;
	rde_point _origin = { 0, 0 };
	arrput(_available_points, _origin);

	rde_atlas_rect* _rects_cpy = NULL;
	
	for(int _i = 0; _i < arrlen(_rects); _i++) {
		arrput(_rects_cpy, _rects[_i]);
	}

	rde_atlas_rect* _packed_rects = NULL;

	while(arrlen(_packed_rects) != arrlen(_rects)) {

		bool _rect_found = false;

		rde_atlas_rect _selected_rect;
		rde_point _selected_point;

		for(int _i = 0; _i < arrlen(_available_points); _i++) {
			rde_point* _point = &_available_points[_i];

			for (int _i = 0; _i < arrlen(_rects_cpy); _i++) {
				rde_atlas_rect* _rect = &_rects_cpy[_i];

				if ((_point->x + _rect->w) > _max_image_size || (_point->y + _rect->h) > _max_image_size) {
					continue;
				}
				
				bool _good_to_pack = true;
				for(int _i = 0; _i < arrlen(_packed_rects); _i++) {
					rde_atlas_rect* _packed_rect = &_packed_rects[_i];

					rde_atlas_rect _rec_0 = *_packed_rect;
					rde_atlas_rect _rec_1;
					_rec_1.x = _point->x;
					_rec_1.y = _point->y;
					_rec_1.w = _rect->w;
					_rec_1.h = _rect->h;

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

				_selected_rect = *_rect;
				_selected_point = *_point;
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

		int _prev_x = _selected_rect.x;
		int _prev_y = _selected_rect.y;
		_selected_rect.x = _selected_point.x;
		_selected_rect.y = _selected_point.y;
		_selected_rect.was_packed = true;

		arrput(_packed_rects, _selected_rect);
		printf("Packed %s \n", _selected_rect.name);

		rde_point _bottom_left_point = { _selected_rect.x + _pixels_of_separation, _selected_rect.y + _selected_rect.h + _pixels_of_separation };
		rde_point _bottom_right_point = { _selected_rect.x + _selected_rect.w + _pixels_of_separation, _selected_rect.y + _selected_rect.h + _pixels_of_separation };
		rde_point _top_right_point = { _selected_rect.x + _selected_rect.w + _pixels_of_separation, _selected_rect.y + _pixels_of_separation };

		for(int _i = arrlen(_rects_cpy) - 1; _i >= 0; _i--) {
			rde_atlas_rect* _r = &_rects_cpy[_i];
			if(_prev_x == _r->x && _prev_y == _r->y && _selected_rect.w == _r->w && _selected_rect.h == _r->h && strcmp(_selected_rect.name, _r->name) == 0) {
				arrdel(_rects_cpy, _i);
				break;
			}
		}

		for(int _i = arrlen(_available_points) - 1; _i >= 0; _i--) {
			rde_point* _p = &_available_points[_i];
			if(_selected_point.x == _p->x && _selected_point.y == _p->y) {
				arrdel(_available_points, _i);
			}
		}

		arrput(_available_points, _bottom_left_point);
		arrput(_available_points, _bottom_right_point);
		arrput(_available_points, _top_right_point);

		qsort(_available_points, arrlen(_available_points), sizeof(rde_point), cmp_func_points);

		for(int _i = 0; _i < arrlen(_packed_rects); _i++) {
			rde_atlas_rect* _packed_rect = &_packed_rects[_i];
			for(int _i = arrlen(_available_points) - 1; _i >= 0; _i--) {
				rde_point* _p = &_available_points[_i];
				if(_p->x > _packed_rect->x && _p->x < _packed_rect->x + _packed_rect->w && _p->y > _packed_rect->y && _p->y < _packed_rect->y + _packed_rect->h) {
					arrdel(_available_points, _i);
				}
			}
		}
	}

	arrfree(_rects);
	_rects = NULL;

	for(int _i = 0; _i < arrlen(_packed_rects); _i++) {
		arrput(_rects, _packed_rects[_i]);
	}

	return true;
}

rde_atlas_rect* load_all_textures_of_atlas(const char* _dir_path) {
	(void)_dir_path;
	rde_atlas_rect* _rects = NULL;

	DIR* _directory = NULL;
	if ((_directory = opendir(_dir_path)) == NULL) {
        fprintf(stderr, "Can't open %s\n", _dir_path);
        exit(-1);
    }

    struct dirent* _entry = NULL;
	while ((_entry = readdir(_directory)) != NULL) {
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
            if(_texture != NULL) {
            	rde_atlas_rect _rect;
            	_rect.pixels = _texture;
            	_rect.w = _width;
            	_rect.h = _height;
				_rect.x = 0;
				_rect.y = 0;

            	memset(_rect.name, 0, 256);
				strcpy(_rect.name, _file_name);
            	arrput(_rects, _rect);
            } else {
            	printf("ERROR: could not load '%s' \n", _file_name);
            }
        }
    }

    closedir(_directory);

    printf("Total textures: %zu \n", arrlen(_rects));

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

	rde_atlas_rect* _rects = load_all_textures_of_atlas(_dir_path);
	
	bool _success = generate_atlas_data(_rects, _max_image_size, _pixels_of_separation);
	while(!_success) {
		printf("Warning: Could not fit everything in a %dx%d atlas, so trying on a %dx%d \n", _max_image_size, _max_image_size, _max_image_size << 1, _max_image_size << 1);
		_max_image_size = _max_image_size << 1;
		_success = generate_atlas_data(_rects, _max_image_size, _pixels_of_separation);
	}

	dump_atlas_to_final_texture(_rects, _atlas_name, _max_image_size);
	printf("Successfully generated png atlas. \n");
	dump_atlas_to_config_file(_rects, _atlas_name);
	printf("Successfully generated atlas data file. \n");

	for(size_t _i = 0; _i < arrlen(_rects); _i++) {
		stbi_image_free(_rects[_i].pixels);
	}

	arrfree(_rects);
}

#pragma clang diagnostic pop