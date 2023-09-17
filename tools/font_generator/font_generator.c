#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-but-set-parameter"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-parameter"

#include <ft2build.h>
#include "freetype/freetype.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#if _WIN32
#define STBIW_WINDOWS_UTF8
#endif
#include "stb/stb_image_write.h"
#include "json/cJSON.c"

#include <stdio.h>
#include <string.h>

#define FONT_DPI 96
#define BASE_FONT_SIZE_FOR_CHAR_SIZE 64
#define CHARACTERS_START_OFFSET 32
#define DEFAULT_FONT_SIZE 36
#define NUMBER_OF_CHARACTERS 128

FT_Library ft_library;
FT_Face ft_face;

int atlas_width = 0;
int atlas_height = 0;

#if _WIN32
int loop_start = CHARACTERS_START_OFFSET;
#else
int loop_start = CHARACTERS_START_OFFSET + 1;
#endif

typedef struct {
	int width;
	int rows;
	FT_Int bitmap_top;
	FT_Int bitmap_left;
	unsigned char* buffer;
	FT_Vector advance;
} inner_bitmap_data;

inner_bitmap_data _glyphs[NUMBER_OF_CHARACTERS - CHARACTERS_START_OFFSET];

void get_filename_without_extension_from_path(const char* _file_path, char* _name_without_extension) {
	const char _path_delimiter = '/';
	char _file_name_with_extension[256] = { 0 };
	char* _file_name_without_extension = NULL;

	const char* _result = strrchr(_file_path, _path_delimiter);

	if(_result != NULL) {
		strcpy(_file_name_with_extension, (_result + 1));
	} else {
		strcpy(_file_name_with_extension, _file_path);
	}

	const char _file_delimiter[] = ".";

	char _file_name_with_extension_arr[256];
	memset(_file_name_with_extension_arr, 0, 256);
	strcpy(_file_name_with_extension_arr, _file_name_with_extension);
	_file_name_without_extension = strtok(_file_name_with_extension_arr, _file_delimiter);

	strcpy(_name_without_extension, _file_name_without_extension);
}

void get_filename_with_extension_from_path(const char* _file_path, char* _name_with_extension) {
	const char _path_delimiter = '/';
	char* _file_name_with_extension = NULL;

	const char* _result = strrchr(_file_path, _path_delimiter);

	if(_result != NULL) {
		strcpy(_file_name_with_extension, (_result + 1));
	} else {
		strcpy(_file_name_with_extension, _file_path);
	}

	strcpy(_name_with_extension, _file_name_with_extension);
}

void load_glyps() {
	int _row_height = 0;
	atlas_width = 0;
	atlas_height = 0;

	FT_Int32 load_flags = FT_LOAD_RENDER | FT_LOAD_TARGET_(FT_RENDER_MODE_SDF);

	for (int _i = loop_start; _i < NUMBER_OF_CHARACTERS; _i++) {
		FT_Error _error_code = FT_Load_Char(ft_face, _i, load_flags);
		if (_error_code != FT_Err_Ok) {
			printf("Loading character %c failed! Error code %d", (char)_i, _error_code);
			continue;
		}
            
		_glyphs[_i - CHARACTERS_START_OFFSET] = (inner_bitmap_data) {
			ft_face->glyph->bitmap.width,
			ft_face->glyph->bitmap.rows,
			ft_face->glyph->bitmap_top,
			ft_face->glyph->bitmap_left,
			NULL,
			ft_face->glyph->advance
		};
            
		inner_bitmap_data* _glyph = &_glyphs[_i - CHARACTERS_START_OFFSET];
		_glyph->buffer = (unsigned char*)malloc(sizeof(unsigned char) * (_glyph->width * _glyph->rows));
		
		if(_glyph->buffer == NULL) {
			printf("Could not allocate memory for character %c \n", (char)_i);
			exit(-1);
		}
		
		memcpy(_glyph->buffer, ft_face->glyph->bitmap.buffer, _glyph->width * _glyph->rows);
            
		atlas_width += ft_face->glyph->bitmap.width;
		_row_height = _row_height > _glyph->rows ? _row_height : (int)_glyph->rows;
	}

	atlas_height += _row_height;
}

void export_font_atlas_texture() {

}

void export_font_atlas_data(const char* _font_path) {
	cJSON* _font_atlas_json = cJSON_CreateObject();
	int _ox = 0;
	int _oy = 0;

	for (int _i = loop_start; _i < NUMBER_OF_CHARACTERS; _i++) {
		inner_bitmap_data* _glyph = &_glyphs[_i - CHARACTERS_START_OFFSET];

		cJSON* _character_json = cJSON_CreateObject();
		char _char_value[8];
		sprintf(_char_value, "%c", (char)_i);
		
		cJSON* _size_json = cJSON_CreateArray();
		cJSON* _size_json_x = cJSON_CreateNumber((double)_glyph->width);
		cJSON* _size_json_y = cJSON_CreateNumber((double)_glyph->rows);
		cJSON_AddItemToArray(_size_json, _size_json_x);
		cJSON_AddItemToArray(_size_json, _size_json_y);

		cJSON* _bearing_json = cJSON_CreateArray();
		cJSON* _bearing_json_x = cJSON_CreateNumber((double)_glyph->bitmap_left);
		cJSON* _bearing_json_y = cJSON_CreateNumber((double)_glyph->bitmap_top);
		cJSON_AddItemToArray(_bearing_json, _bearing_json_x);
		cJSON_AddItemToArray(_bearing_json, _bearing_json_y);

		cJSON* _offset_json = cJSON_CreateArray();
		cJSON* _offset_json_x = cJSON_CreateNumber((double)((float)_ox / (float)atlas_width));
		cJSON* _offset_json_y = cJSON_CreateNumber((double)((float)_oy / (float)atlas_height));
		cJSON_AddItemToArray(_offset_json, _offset_json_x);
		cJSON_AddItemToArray(_offset_json, _offset_json_y);

		cJSON* _advance_json = cJSON_CreateArray();
		cJSON* _advance_json_x = cJSON_CreateNumber((double)((int)_glyph->advance.x >> 6));
		cJSON* _advance_json_y = cJSON_CreateNumber((double)((double)_glyph->rows));
		cJSON_AddItemToArray(_advance_json, _advance_json_x);
		cJSON_AddItemToArray(_advance_json, _advance_json_y);

		cJSON_AddItemToObject(_character_json, "advance", _advance_json);
		cJSON_AddItemToObject(_character_json, "bearing", _bearing_json);
		cJSON_AddItemToObject(_character_json, "offset", _offset_json);
		cJSON_AddItemToObject(_character_json, "size", _size_json);

		cJSON_AddItemToObject(_font_atlas_json, _char_value, _character_json);

		_ox += (int)_glyph->width;
	}

	const char* _stringify_json = cJSON_Print(_font_atlas_json);
	
	char _file_name_without_extension[256];
	memset(_file_name_without_extension, 0, 256);

	char* _custom_extension = ".rde_font";
	get_filename_without_extension_from_path(_font_path, _file_name_without_extension);
	strncat(_file_name_without_extension, _custom_extension, strlen(_custom_extension));

	FILE* _file = fopen(_file_name_without_extension, "w");
	
	if (_file == NULL) {
		printf("Error: could not create the output file for the data \n");
		exit(-1);
	}

	fprintf(_file, "%s", _stringify_json);
	fclose(_file);

	cJSON_Delete(_font_atlas_json);
}

int main(int _argc, char** _argv) {
	
	if(_argc != 2) {
		printf("Error: first parameter = path to the font \n");
		exit(-1);
	}

	const char* _font_path = _argv[1];
	
	if(FT_Init_FreeType(&ft_library)) {
		printf("Error initiating FreeType \n");
		exit(-1);
	}

	FT_Error _error = FT_New_Face(ft_library, _font_path, 0, &ft_face);
	if(_error != FT_Err_Ok) {
		printf("Error loading FT_Face -> %d \n", _error);
		exit(-1);
	}

	FT_Set_Char_Size(ft_face, 0, DEFAULT_FONT_SIZE * BASE_FONT_SIZE_FOR_CHAR_SIZE, FONT_DPI, FONT_DPI);
	
	load_glyps();
	export_font_atlas_texture();
	export_font_atlas_data(_font_path);

	printf("Generated atlas texture and data file correctly!");
}

#pragma GCC diagnostic pop
#pragma clang diagnostic pop