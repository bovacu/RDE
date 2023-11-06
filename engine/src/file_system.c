#ifdef RDE_RENDERING_MODULE

rde_atlas_sub_textures* rde_inner_file_system_read_atlas_config(const char* _atlas_path, rde_texture* _atlas);
rde_font_char_info* rde_inner_file_system_read_font_config(const char* _font_path, rde_texture* _atlas);

rde_atlas_sub_textures* rde_inner_file_system_read_atlas_config(const char* _atlas_path, rde_texture* _atlas) {
	FILE* _file = NULL;
	char* _text = NULL;

	#if IS_WINDOWS()
		errno_t _err = fopen_s(&_file, _atlas_path, "r");
		rde_critical_error(_err != 0, RDE_ERROR_FILE_NOT_FOUND, _atlas_path);
	#else
		_file = fopen(_atlas_path, "r");
		rde_critical_error(_file == NULL, RDE_ERROR_FILE_NOT_FOUND, _atlas_path);
	#endif

	long _num_bytes = 0;
	fseek(_file, 0L, SEEK_END);
	_num_bytes = ftell(_file);
	fseek(_file, 0L, SEEK_SET);

	_text = (char*)calloc(_num_bytes, sizeof(char));
	rde_critical_error(_text == NULL, RDE_ERROR_NO_MEMORY, _num_bytes, "config file");
	fread(_text, sizeof(char), _num_bytes, _file);


	rde_atlas_sub_textures* hash = NULL;
	cJSON* _atlas_json = cJSON_Parse(_text);

	if(_atlas_json == NULL) {
		const char* _error_ptr = cJSON_GetErrorPtr();
		rde_critical_error(true, RDE_ERROR_JSON, _atlas_path, _error_ptr);
	}

	cJSON* _sub_texture_json = NULL;
	cJSON_ArrayForEach(_sub_texture_json, _atlas_json) {
		cJSON* _position = cJSON_GetObjectItemCaseSensitive(_sub_texture_json, "position");
		cJSON* _size = cJSON_GetObjectItemCaseSensitive(_sub_texture_json, "size");

		rde_texture _texture;
		_texture.opengl_texture_id = _atlas->opengl_texture_id;
		_texture.size.x = (uint)cJSON_GetArrayItem(_size, 0)->valueint;
		_texture.size.y = (uint)cJSON_GetArrayItem(_size, 1)->valueint;
		_texture.position.x = (uint)cJSON_GetArrayItem(_position, 0)->valueint;
		_texture.position.y = (uint)cJSON_GetArrayItem(_position, 1)->valueint;
		_texture.channels = _atlas->channels;
		_texture.internal_format = _atlas->internal_format;
		_texture.data_format = _atlas->data_format;
		_texture.file_path = _atlas->file_path;
		_texture.atlas_texture = _atlas;
		stbds_shput(hash, _sub_texture_json->string, _texture);

		cJSON_free(_position);
		cJSON_free(_size);
	}

	cJSON_free(_atlas_json);

	fclose(_file);
	free(_text);

	return hash;
}

rde_font_char_info* rde_inner_file_system_read_font_config(const char* _font_path, rde_texture* _atlas) {
	FILE* _file = NULL;
	char* _text = NULL;

	#if IS_WINDOWS()
	errno_t _err = fopen_s(&_file, _font_path, "r");
	rde_critical_error(_err != 0, RDE_ERROR_FILE_NOT_FOUND, _font_path);
	#else
	_file = fopen(_font_path, "r");
	rde_critical_error(_file == NULL, RDE_ERROR_FILE_NOT_FOUND, _font_path);
	#endif

	long _num_bytes = 0;
	fseek(_file, 0L, SEEK_END);
	_num_bytes = ftell(_file);
	fseek(_file, 0L, SEEK_SET);

	_text = (char*)calloc(_num_bytes, sizeof(char));
	rde_critical_error(_text == NULL, RDE_ERROR_NO_MEMORY, _num_bytes, "config file");
	fread(_text, sizeof(char), _num_bytes, _file);


	rde_font_char_info* _chars = NULL;
	cJSON* _font_json = cJSON_Parse(_text);

	if(_font_json == NULL) {
		const char* _error_ptr = cJSON_GetErrorPtr();
		rde_critical_error(true, RDE_ERROR_JSON, _font_path, _error_ptr);
	}

	cJSON* _char_info_json = NULL;
	rde_font_char_info _fci_default = rde_struct_create_font_char_info();
	cJSON* _extra_data = cJSON_GetObjectItemCaseSensitive(_font_json, "extra_data");
	cJSON* _offset_from_start = cJSON_GetObjectItemCaseSensitive(_extra_data, "offset_from_start");
	for(int _i = 0; _i < _offset_from_start->valueint; _i++) {
		stbds_arrput(_chars, _fci_default);
	}

	cJSON* _characters = cJSON_GetObjectItemCaseSensitive(_font_json, "characters");

	cJSON_ArrayForEach(_char_info_json, _characters) {
		cJSON* _advance = cJSON_GetObjectItemCaseSensitive(_char_info_json, "advance");
		cJSON* _size = cJSON_GetObjectItemCaseSensitive(_char_info_json, "size");
		cJSON* _bearing = cJSON_GetObjectItemCaseSensitive(_char_info_json, "bearing");
		cJSON* _offset = cJSON_GetObjectItemCaseSensitive(_char_info_json, "offset");
		cJSON* _metrics = cJSON_GetObjectItemCaseSensitive(_char_info_json, "metrics");

		rde_font_char_info _char_info = rde_struct_create_font_char_info();
		_char_info.advance = (rde_vec_2I) { cJSON_GetArrayItem(_advance, 0)->valueint, cJSON_GetArrayItem(_advance, 1)->valueint };
		_char_info.size = (rde_vec_2I) { cJSON_GetArrayItem(_size, 0)->valueint, cJSON_GetArrayItem(_size, 1)->valueint };
		_char_info.bearing = (rde_vec_2I) { cJSON_GetArrayItem(_bearing, 0)->valueint, cJSON_GetArrayItem(_bearing, 1)->valueint };
		_char_info.offset = (rde_vec_2I) { cJSON_GetArrayItem(_offset, 0)->valueint, cJSON_GetArrayItem(_offset, 1)->valueint };
		_char_info.metrics = (rde_vec_2I) { cJSON_GetArrayItem(_metrics, 0)->valueint, cJSON_GetArrayItem(_metrics, 1)->valueint };

		rde_texture _texture;
		_texture.opengl_texture_id = _atlas->opengl_texture_id;
		_texture.size.x = _char_info.size.x;
		_texture.size.y = _char_info.size.y;
		_texture.position.x = _char_info.offset.x;
		_texture.position.y = _char_info.offset.y;
		_texture.channels = _atlas->channels;
		_texture.internal_format = _atlas->internal_format;
		_texture.data_format = _atlas->data_format;
		_texture.file_path = _atlas->file_path;
		_texture.atlas_texture = _atlas;
		_char_info.texture = _texture;

		stbds_arrput(_chars, _char_info);

		cJSON_free(_advance);
		cJSON_free(_size);
		cJSON_free(_bearing);
		cJSON_free(_offset);
	}

	cJSON_free(_font_json);

	fclose(_file);
	free(_text);

	return _chars;
}
#endif

static const char* rde_inner_file_system_file_mode_to_char(const RDE_FILE_MODE_ _mode) {
	switch (_mode) {
		case RDE_FILE_MODE_READ: return "rb";
		case RDE_FILE_MODE_WRITE: return "w";
		case RDE_FILE_MODE_APPEND: return "a";
		case RDE_FILE_MODE_READ_AND_WRITE: return "r+";
		case RDE_FILE_MODE_READ_AND_APPEND: return "a+";
	}

	return "rb";
}

static void rde_inner_file_system_free_text_allocation(rde_file_handle* _handler) {
	if(_handler->text_allocated != NULL) {
		free(_handler->text_allocated);
		_handler->text_allocated = NULL;
	}
}

static void rde_inner_file_system_check_file_mode_or_convert(rde_file_handle* _handler, RDE_FILE_MODE_ _expected) {
	if(_handler->file_mode == _expected) return;
	SDL_RWclose(_handler->sdl_handle);
	rde_inner_file_system_free_text_allocation(_handler);
	_handler->sdl_handle = SDL_RWFromFile(_handler->file_path, rde_inner_file_system_file_mode_to_char(_expected));
	_handler->file_mode = _expected;
}

#define RDE_MAX_CONCURRENT_FILES_OPENED 64
static rde_file_handle concurrent_file_handlers[RDE_MAX_CONCURRENT_FILES_OPENED];




// ==============================================================================
// =									API										=
// ==============================================================================

rde_file_handle* rde_file_open(const char* _file_path, RDE_FILE_MODE_ _file_mode) {
	const char* _mode = rde_inner_file_system_file_mode_to_char(_file_mode);
	SDL_RWops* _sdl_file = SDL_RWFromFile(_file_path, _mode);
	rde_critical_error(_sdl_file == NULL, RDE_ERROR_FILE_NOT_FOUND, _file_path);

	for(size_t _i = 0; _i < RDE_MAX_CONCURRENT_FILES_OPENED; _i++) {
		rde_file_handle* _file_handler = &concurrent_file_handlers[_i];

		if(_file_handler->sdl_handle == NULL) {
			_file_handler->sdl_handle = _sdl_file;
			_file_handler->file_mode = _file_mode;
			memset(_file_handler->file_path, 0, RDE_MAX_PATH);
#if IS_WINDOWS()
			strcat_s(_file_handler->file_path, RDE_MAX_PATH, _file_path);
#else
			strcat(_file_handler->file_path, _file_path);
#endif
			_file_handler->text_allocated = NULL;

			return _file_handler;
		}
	}

	rde_critical_error(true, RDE_ERROR_FILE_SYSTEM_MAX_OPENED_FILES_REACHED, RDE_MAX_CONCURRENT_FILES_OPENED);
	return NULL;
}

char* rde_file_read_full_file(rde_file_handle* _file_handler) {
	rde_inner_file_system_check_file_mode_or_convert(_file_handler, RDE_FILE_MODE_READ);
	size_t _total_size = SDL_RWsize(_file_handler->sdl_handle);
	size_t _total_bytes_read = 0;
	size_t _bytes_to_read = 1;
	char* _text = (char*)malloc(sizeof(char) * _total_size + 1);
	char* _buf = _text;
	memset(_text, 0, _total_size);
	rde_critical_error(_text == NULL, RDE_ERROR_NO_MEMORY, sizeof(char) * _total_size, "Text Read Full File");

	while (_total_bytes_read < _total_size && _bytes_to_read != 0) {
		_bytes_to_read = SDL_RWread(_file_handler->sdl_handle, _buf, 1, (_total_size - _total_bytes_read));
		_total_bytes_read += _bytes_to_read;
		_buf += _bytes_to_read;
	}
	_text[_total_bytes_read] = '\0';

	rde_inner_file_system_free_text_allocation(_file_handler);
	_file_handler->text_allocated = _text;
	return _text;
}

char* rde_file_read_line(rde_file_handle* _file_handler, size_t _line) {
	size_t _current_line = 0;
	size_t _final_line_ptr = 0;
	bool _line_found = false;
	size_t _line_first_byte = RDE_UINT_MAX;

	rde_inner_file_system_check_file_mode_or_convert(_file_handler, RDE_FILE_MODE_READ);
	SDL_RWseek(_file_handler->sdl_handle, 0, RW_SEEK_END);
	long _content_size = SDL_RWtell(_file_handler->sdl_handle);
	char* _file_content = (char*)malloc(sizeof(char) * _content_size);
	SDL_RWseek(_file_handler->sdl_handle, 0, RW_SEEK_SET);

	for(long _i = 0; _i < _content_size; _i++) {
		SDL_RWread(_file_handler->sdl_handle, &_file_content[_i], sizeof(char), 1);
		if(_current_line == _line) {
			
			if(_line_first_byte == RDE_UINT_MAX) {
				_line_first_byte = _i;
			}

			_line_found = true;
			if(_file_content[_i] == '\n' || (_file_content[_i] == '\r' && _file_content[_i + 1] == '\n')) break;
			_final_line_ptr++;
			continue;
		}

		if(_file_content[_i] == '\n') _current_line++;
	}

	if(!_line_found) {
		return NULL;
	}

	char* _line_ptr = (char*)malloc(sizeof(char) * _final_line_ptr);
	memset(_line_ptr, 0, _final_line_ptr);
#if IS_WINDOWS()
	strncpy_s(_line_ptr, _final_line_ptr, _file_content, _final_line_ptr);
#else
	strncpy(_line_ptr, _file_content, _final_line_ptr);
#endif

	rde_inner_file_system_free_text_allocation(_file_handler);
	_file_handler->text_allocated = _line_ptr;
	return _line_ptr;
}

char* rde_file_read_chunk(rde_file_handle* _file_handler, size_t _begin_byte, size_t _end_byte) {
	UNUSED(_file_handler)
	UNUSED(_begin_byte)
	UNUSED(_end_byte)
	UNIMPLEMENTED("rde_file_read_chunk")
	return NULL;
}

void rde_file_write(rde_file_handle* _file_handler, size_t _bytes, const char* _data) {
	UNUSED(_file_handler)
	UNUSED(_bytes)
	UNUSED(_data)
	UNIMPLEMENTED("rde_file_write")
}

void rde_file_write_to_line(rde_file_handle* _file_handler, size_t _bytes, const char* _data, size_t _line) {
	UNUSED(_file_handler)
	UNUSED(_bytes)
	UNUSED(_data)
	UNUSED(_line)
	UNIMPLEMENTED("rde_file_write_to_line")
}

void rde_file_append(rde_file_handle* _file_handler, size_t _append_byte, size_t _bytes, const char* _data, size_t _line) {
	UNUSED(_file_handler)
	UNUSED(_append_byte)
	UNUSED(_bytes)
	UNUSED(_data)
	UNUSED(_line)
	UNIMPLEMENTED("rde_file_append")
}

void rde_file_clear_content(rde_file_handle* _file_handler) {
	UNUSED(_file_handler)
	UNIMPLEMENTED("rde_file_clear_content")
}

bool rde_file_exists(const char* _file_path) {
	UNUSED(_file_path)
	UNIMPLEMENTED("rde_file_exists")
	return false;
}

void rde_file_delete(const char* _file_path) {
	UNUSED(_file_path)
	UNIMPLEMENTED("rde_file_delete")
}

void rde_file_move(const char* _file_path, const char* _new_file_path) {
	UNUSED(_file_path)
	UNUSED(_new_file_path)
	UNIMPLEMENTED("rde_file_move")
}

void rde_file_close(rde_file_handle* _file_handler) {
	rde_inner_file_system_free_text_allocation(_file_handler);
	SDL_RWclose(_file_handler->sdl_handle);
	_file_handler->sdl_handle = NULL;
}