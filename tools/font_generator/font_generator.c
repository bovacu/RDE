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

char font_path[256];
int number_of_characters = NUMBER_OF_CHARACTERS;
int font_size = DEFAULT_FONT_SIZE;
int font_dpi = FONT_DPI;
int font_char_start_offset = CHARACTERS_START_OFFSET;

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
	char char_name;
	int pitch;
} inner_bitmap_data;

inner_bitmap_data* _glyphs;

void print_help() {
	char* _help_message = "This command line program transforms a font file into a usable font atlas for the RDE engine. \n"
		"Mandatory arguments: \n"
		"	--font_path (or -p): Path to the font file \n"
		"\n"
		"Optional arguments: \n"
		"	--font_num_of_characters= (or -c=): Number of characters to be parsed, if not provided 128 is the default value \n"
		"	--font_size= (or -s=): Size of the font, if not provided 36 is the default value \n"
		"	--font_dpi= (or -d=): Size of the dpi of the font, if not provided 96 is the default value \n"
		"	--font_char_offset= (or -o=): Number of characters that will be skipped from the beggining, if not provided 32 is the default \n"
		;

	printf("%s \n", _help_message);
}

void process_command_line_arguments(int _argc, char** _argv) {
	if(_argc < 2) {
		print_help();
		exit(-1);
	}

	const char* _delimiter = "=";
	const char _delimiter_2 = '=';
	for(int _i = 1; _i < _argc; _i++) {
		char _command_copy[256];
		memset(_command_copy, 0, 256);
		strcpy(_command_copy, _argv[_i]);
		char* _command = strtok(_command_copy, _delimiter);
		
		// TODO: change atoi for strtol
		if(strcmp(_command, "-p") == 0 || strcmp(_command, "--font_path") == 0) {
			const char* _value = strrchr(_argv[_i], _delimiter_2);
			strcpy(font_path, (_value + 1));
		} else if(strcmp(_command, "-c") == 0 || strcmp(_command, "--font_num_of_characters") == 0) {
			const char* _value = strrchr(_argv[_i], _delimiter_2);
			number_of_characters = atoi((_value + 1));
		} else if(strcmp(_command, "-s") == 0 || strcmp(_command, "--font_size") == 0) {
			const char* _value = strrchr(_argv[_i], _delimiter_2);
			font_size = atoi((_value + 1));
		} else if(strcmp(_command, "-d") == 0 || strcmp(_command, "--font_dpi") == 0) {
			const char* _value = strrchr(_argv[_i], _delimiter_2);
			font_dpi = atoi((_value + 1));
		} else if(strcmp(_command, "-o") == 0 || strcmp(_command, "--font_char_offset") == 0) {
			const char* _value = strrchr(_argv[_i], _delimiter_2);
			font_char_start_offset = atoi((_value + 1));
		} else if(strcmp(_command, "-h") == 0 || strcmp(_command, "--help") == 0) {
			print_help();
			exit(0);
		}
	}

	if(strlen(font_path) == 0) {
		print_help();
		exit(-1);
	}
}

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

	for (int _i = loop_start; _i < number_of_characters; _i++) {
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
			ft_face->glyph->advance,
			(char)(_i - CHARACTERS_START_OFFSET),
			ft_face->glyph->bitmap.pitch
		};
            
		inner_bitmap_data* _glyph = &_glyphs[_i - font_char_start_offset];
		_glyph->buffer = (unsigned char*)malloc(sizeof(unsigned char) * (_glyph->width * _glyph->rows));

		if(_glyph->buffer == NULL) {
			printf("Could not allocate memory for character %c \n", (char)_i);
			exit(-1);
		}
		
		memset(_glyph->buffer, 0, _glyph->width * _glyph->rows);
		memcpy(_glyph->buffer, ft_face->glyph->bitmap.buffer, _glyph->width * _glyph->rows);
            
		atlas_width += ft_face->glyph->bitmap.width;
		_row_height = _row_height > _glyph->rows ? _row_height : (int)_glyph->rows;
	}

	atlas_height += _row_height;
}

void export_font_atlas_texture() {

	char _full_name[256];
	char* _custom_extension = ".png";
	get_filename_without_extension_from_path(font_path, _full_name);
	strncat(_full_name, _custom_extension, strlen(_custom_extension));

	unsigned char* _atlas_pixels = (unsigned char*)malloc(sizeof(unsigned char) * atlas_width * atlas_height);
	memset(_atlas_pixels, 0, atlas_width * atlas_height);
	
	int _ox = 0;
	int _oy = 0;
	int _row_height = 0;

	for (int _i = loop_start; _i < number_of_characters; _i++) {
		inner_bitmap_data* _glyph = &_glyphs[_i - font_char_start_offset];
		
		if(_ox + _glyph->width > atlas_width) {
			_oy += _row_height;
			_ox = 0;
			_row_height = 0;
		}

		// NOTE: origin is top left (0, 0), being bottom right the (_max_image_size - 1, _max_image_size - 1)

		for (int _py = 0; _py < _glyph->rows; _py++) {
			int _height_offset = atlas_width * _py;

			for(int _px = 0; _px < _glyph->width; _px++) {
				_atlas_pixels[(_ox + _oy * atlas_width) + _height_offset + _px] = _glyph->buffer[_px + _glyph->width * _py];
			}
		}
		_ox += (int)_glyph->width;
		_row_height = _row_height > _glyph->rows ? _row_height : (int)_glyph->rows;
	}

	if(stbi_write_png(_full_name, atlas_width, atlas_height, 1, _atlas_pixels, sizeof(unsigned char) * atlas_width) == 0) {
		free(_atlas_pixels);
		printf("Error writting atlas png \n");
		exit(-1);
	}

	free(_atlas_pixels);
}

void export_font_atlas_data() {
	cJSON* _font_atlas_json = cJSON_CreateObject();
	int _ox = 0;
	int _oy = 0;
	int _row_height;

	for (int _i = loop_start; _i < number_of_characters; _i++) {
		inner_bitmap_data* _glyph = &_glyphs[_i - font_char_start_offset];

		if(_ox + _glyph->width > atlas_width) {
			_oy += _row_height;
			_ox = 0;
			_row_height = 0;
		}

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
		_row_height = _row_height > _glyph->rows ? _row_height : (int)_glyph->rows;
	}

	const char* _stringify_json = cJSON_Print(_font_atlas_json);
	
	char _file_name_without_extension[256];
	memset(_file_name_without_extension, 0, 256);

	char* _custom_extension = ".rde_font";
	get_filename_without_extension_from_path(font_path, _file_name_without_extension);
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
	process_command_line_arguments(_argc, _argv);
	
	if(FT_Init_FreeType(&ft_library)) {
		printf("Error initiating FreeType \n");
		exit(-1);
	}

	FT_Error _error = FT_New_Face(ft_library, font_path, 0, &ft_face);
	if(_error != FT_Err_Ok) {
		printf("Error loading FT_Face -> %d \n", _error);
		exit(-1);
	}

	// Multiply by BASE_FONT_SIZE_FOR_CHAR_SIZE is needed as sizes on FreeType are on a 1/64 base.
	FT_Set_Char_Size(ft_face, 0, font_size * BASE_FONT_SIZE_FOR_CHAR_SIZE, font_dpi, font_dpi);
	
	_glyphs = (inner_bitmap_data*)malloc(sizeof(inner_bitmap_data) * (number_of_characters - font_char_start_offset));
	load_glyps();

	size_t _atlas_area = atlas_width * atlas_height;

	size_t _power_of_two_size = 4;
	while(_power_of_two_size * _power_of_two_size < _atlas_area) {
		_power_of_two_size = _power_of_two_size << 1;
	}

	atlas_width = _power_of_two_size;
	atlas_height = _power_of_two_size;

	export_font_atlas_texture();
	export_font_atlas_data();

	free(_glyphs);

	printf("Generated atlas texture and data file correctly!");
}

#pragma GCC diagnostic pop
#pragma clang diagnostic pop