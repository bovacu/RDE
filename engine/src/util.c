#define RDE_IMPLEMENT_SAFE_ARR_ACCESS(_type)																								\
_type rde_arr_s_get_##_type(unsigned int _index, _type* _arr, size_t _arr_size, char* _fmt, ...) {											\
	if(_index >= _arr_size) {																												\
		va_list _args;																														\
		va_start(_args, _fmt);																												\
		vfprintf(stdout, _fmt, _args);																										\
		va_end(_args);																														\
		printf("\n");																														\
	}																																		\
	rde_critical_error(_index >= _arr_size, "Index accessed '%u' is greater than array size '%llu' in '%s'.\n", _index, _arr_size);			\
	return _arr[_index];																													\
}
#define RDE_IMPLEMENT_SAFE_ARR_SET(_type)																									\
void rde_arr_s_set_##_type(unsigned int _index, _type _value, _type* _arr, size_t _arr_size, char* _fmt, ...) {								\
	if(_index >= _arr_size) {																												\
		va_list _args;																														\
		va_start(_args, _fmt);																												\
		vfprintf(stdout, _fmt, _args);																										\
		va_end(_args);																														\
		printf("\n");																														\
	}																																		\
	rde_critical_error(_index >= _arr_size, "Index accessed '%u' is greater than array size '%llu''.\n", _index, _arr_size);				\
	_arr[_index] = _value;																													\
}

RDE_IMPLEMENT_SAFE_ARR_ACCESS(int)
RDE_IMPLEMENT_SAFE_ARR_ACCESS(uint)
RDE_IMPLEMENT_SAFE_ARR_ACCESS(size_t)
RDE_IMPLEMENT_SAFE_ARR_ACCESS(short)
RDE_IMPLEMENT_SAFE_ARR_ACCESS(float)
RDE_IMPLEMENT_SAFE_ARR_ACCESS(double)

RDE_IMPLEMENT_SAFE_ARR_SET(int)
RDE_IMPLEMENT_SAFE_ARR_SET(uint)
RDE_IMPLEMENT_SAFE_ARR_SET(size_t)
RDE_IMPLEMENT_SAFE_ARR_SET(short)
RDE_IMPLEMENT_SAFE_ARR_SET(float)
RDE_IMPLEMENT_SAFE_ARR_SET(double)

void rde_util_sanitaize_path(const char* _path, char* _sanitized, size_t _sanitized_size) {
	rde_critical_error(strlen(_path) > _sanitized_size, RDE_ERROR_SANITIZATION_PATH, _path, _sanitized_size);
	memset(_sanitized, 0, _sanitized_size);

	char _fucking_windows_backslash = '\\';
	char _lovely_unix_slash = '/';
	
	for(size_t _i = 0; _i < strlen(_path); _i++) {
		_sanitized[_i] = _path[_i];
		if(_sanitized[_i] == _fucking_windows_backslash) {
			_sanitized[_i] = _lovely_unix_slash;
		}
	}
}

const char* rde_util_file_get_name_extension(const char* _file_name) {
	const char* _dot = strrchr(_file_name, '.');
	if(!_dot || _dot == _file_name) return "";
	return _dot + 1;
}

rde_file_handler* rde_file_open(const char* _file_path, RDE_FILE_MODE_ _file_mode) {
	UNUSED(_file_path)
	UNUSED(_file_mode)
	UNIMPLEMENTED("rde_file_open")
	return NULL;
}

rde_file_handler* rde_file_open_or_create(const char* _file_path, RDE_FILE_MODE_ _file_mode) {
	UNUSED(_file_path)
	UNUSED(_file_mode)
	UNIMPLEMENTED("rde_file_open_or_create")
	return NULL;
}

char* rde_file_read_full_file(rde_file_handler* _file_handler) {
	UNUSED(_file_handler)
	UNIMPLEMENTED("rde_file_read_full_file")
	return NULL;
}

char* rde_file_read_line(rde_file_handler* _file_handler, size_t _line) {
	UNUSED(_file_handler)
	UNUSED(_line)
	UNIMPLEMENTED("rde_file_read_line")
	return NULL;
}

char* rde_file_read_chunk(rde_file_handler* _file_handler, size_t _begin_byte, size_t _end_byte) {
	UNUSED(_file_handler)
	UNUSED(_begin_byte)
	UNUSED(_end_byte)
	UNIMPLEMENTED("rde_file_read_chunk")
	return NULL;
}

void rde_file_write(rde_file_handler* _file_handler, size_t _bytes, const char* _data) {
	UNUSED(_file_handler)
	UNUSED(_bytes)
	UNUSED(_data)
	UNIMPLEMENTED("rde_file_write")
}

void rde_file_write_to_line(rde_file_handler* _file_handler, size_t _bytes, const char* _data, size_t _line) {
	UNUSED(_file_handler)
	UNUSED(_bytes)
	UNUSED(_data)
	UNUSED(_line)
	UNIMPLEMENTED("rde_file_write_to_line")
}

void rde_file_append(rde_file_handler* _file_handler, size_t _append_byte, size_t _bytes, const char* _data, size_t _line) {
	UNUSED(_file_handler)
	UNUSED(_append_byte)
	UNUSED(_bytes)
	UNUSED(_data)
	UNUSED(_line)
	UNIMPLEMENTED("rde_file_append")
}

void rde_file_clear_content(rde_file_handler* _file_handler) {
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

void rde_file_close(rde_file_handler* _file_handler) {
	UNUSED(_file_handler)
	UNIMPLEMENTED("rde_file_close")
}

#ifdef RDE_RENDERING_MODULE
size_t rde_util_font_get_string_width(const char* _string, const rde_font* _font) {
	int _text_size = strlen(_string);

	size_t _size = 0;
	for (int _i = 0; _i < _text_size; _i++) {
		int _key = (int)_string[_i];
		rde_font_char_info _char_info = _font->chars[_key];
		_size += (_char_info.advance.x >> 6); // /64.f
	}

	return _size;
}

rde_vec_2I rde_util_font_get_string_size(const char* _string, const rde_font* _font) {
	int _text_size = strlen(_string);

	int _width = 0;
	int _height = 0;
	for (int _i = 0; _i < _text_size; _i++) {
		int _key = (int)_string[_i];
		rde_font_char_info _char_info = _font->chars[_key];
		_width += (_char_info.advance.x >> 6);

		if(_char_info.size.y > _height) {
			_height = _char_info.size.y;
		}
	}

	return (rde_vec_2I) { _width, _height };
}
#endif

char* ltrim(char* _s) {
	while(isspace(*_s)) _s++;
	return _s;
}

char* rtrim(char* _s) {
	char* _back = _s + strlen(_s);
	while(isspace(*--_back));
	*(_back+1) = '\0';
	return _s;
}

char* rde_util_string_trim(char* _s) {
	char* _end;

	while(isspace((unsigned char)*_s)) _s++;

	if(*_s == 0) {
		return _s;
	}

	_end = _s + strlen(_s) - 1;
	while(_end > _s && isspace((unsigned char)*_end)) _end--;

	_end[1] = '\0';

	return _s;
}

bool rde_util_string_starts_with(const char* _string, const char* _prefix) {
	if(_string == NULL || _prefix == NULL) {
		return false;
	}
	return strncmp(_string, _prefix, strlen(_prefix)) == 0;
}

bool rde_util_string_ends_with(const char* _string, const char* _suffix) {
	if(_string == NULL || _suffix == NULL) {
		return false;
	}

	size_t _suffix_size = strlen(_suffix);
	size_t _string_size = strlen(_string);
	return strncmp(_string + (_string_size - _suffix_size), _suffix, _suffix_size) == 0;
}

bool rde_util_string_contains_substring(const char* _string, const char* _substring, bool _case_sensitive) {
	if(_string == NULL || _substring == NULL) {
		return false;
	}

	if(_case_sensitive) {
		return strstr(_string, _substring) != NULL;
	} else {
#if IS_WINDOWS()
		return StrStrI(_string, _substring) != NULL;
#else
		return strcasestr(_string, _substring) != NULL;
#endif
	}
	return false;
}

size_t rde_util_string_char_appearences(const char* _string, char _char) {
	if(_string == NULL) {
		return 0;
	}

	size_t _amount = 0;
	size_t _string_size = strlen(_string);
	for(size_t _i = 0; _i < _string_size; _i++) {
		if(_string[_i] == _char) {
			_amount++;
		}
	}

	return _amount;
}

void rde_util_string_concat(char* _string, size_t _size, const char* _fmt, ...) {
	if(_string == NULL) {
		return;
	}

	va_list _args;
	va_start(_args, _fmt);
	vsnprintf(_string, _size, _fmt, _args);
	va_end(_args);
}

void rde_util_string_to_lower(char* _destination, const char* _string) {
	if(_string == NULL || _destination == NULL) {
		return;
	}

	size_t _string_size = strlen(_string);
	for(size_t _i = 0; _i < _string_size; _i++){
		_destination[_i] = tolower(_string[_i]);
	}
}

void rde_util_string_to_lower_itself(char* _string) {
	if(_string == NULL) {
		return;
	}

	size_t _string_size = strlen(_string);
	for(size_t _i = 0; _i < _string_size; _i++){
		_string[_i] = tolower(_string[_i]);
	}
}

void rde_util_string_to_upper(char* _destination, const char* _string) {
	if(_string == NULL || _destination == NULL) {
		return;
	}

	size_t _string_size = strlen(_string);
	for(size_t _i = 0; _i < _string_size; _i++){
		_destination[_i] = toupper(_string[_i]);
	}
}

void rde_util_string_to_upper_itself(char* _string) {
	if(_string == NULL) {
		return;
	}

	size_t _string_size = strlen(_string);
	for(size_t _i = 0; _i < _string_size; _i++){
		_string[_i] = toupper(_string[_i]);
	}
}

void rde_util_string_replace_char(char* _string, char _old, char _new) {
	if(_string == NULL) {
		return;
	}

	size_t _string_size = strlen(_string);
	for(size_t _i = 0; _i < _string_size; _i++) {
		if(_string[_i] == _old) {
			_string[_i] = _new;
			break;
		}
	}
}

void rde_util_string_replace_chars_all(char* _string, char _old, char _new) {
	if(_string == NULL) {
		return;
	}

	size_t _string_size = strlen(_string);
	for(size_t _i = 0; _i < _string_size; _i++) {
		if(_string[_i] == _old) {
			_string[_i] = _new;
		}
	}
}

size_t rde_util_string_split(char* _string, char** _split_array, const char* _split_mark) {
	if(_string == NULL || _split_array == NULL || _split_mark == NULL) {
		return 0;
	}

	size_t _amount = 0;
	
#if IS_WINDOWS()
	char* _string_ptr = strtok_s(_string, _split_mark, NULL);
#else
	char* _string_ptr = strtok(_string, _split_mark);
#endif

	while(_string_ptr != NULL) {
#if IS_WINDOWS()
		size_t _string_ptr_size = strlen(_string_ptr);
		strcat_s(_split_array[_amount], _string_ptr_size, _string_ptr);
		_string_ptr = strtok_s(NULL, _split_mark, NULL);
#else
		strcat(_split_array[_amount], _string_ptr);
		_string_ptr = strtok(NULL, _split_mark);
#endif
		_amount++;
	}

	return _amount;
}

void rde_log_color_inner(RDE_LOG_COLOR_ _color, const char* _fmt, ...) {
	switch(_color) {
		case RDE_LOG_COLOR_RED: {
#if IS_WINDOWS()
			SetConsoleTextAttribute(ENGINE.console_handle, 12);
#else
			fprintf(stdout, "\033[0;31m");
#endif
		} break;
		
		case RDE_LOG_COLOR_GREEN:  {
#if IS_WINDOWS()
			SetConsoleTextAttribute(ENGINE.console_handle, 10);
#else
			fprintf(stdout, "\033[0;32m");	
#endif
		} break;
		
		case RDE_LOG_COLOR_YELLOW:  {
#if IS_WINDOWS()
			SetConsoleTextAttribute(ENGINE.console_handle, 6);
#else
			fprintf(stdout, "\033[0;33m");	
#endif
		} break;
		
		case RDE_LOG_COLOR_BLUE:  {
#if IS_WINDOWS()
			SetConsoleTextAttribute(ENGINE.console_handle, 9);
#else
			fprintf(stdout, "\033[0;34m");		
#endif
		} break;
		
		case RDE_LOG_COLOR_PURPLE:  {
#if IS_WINDOWS()
			SetConsoleTextAttribute(ENGINE.console_handle, 5);
#else
			fprintf(stdout, "\033[0;35m");
#endif
		} break;
		
		case RDE_LOG_COLOR_CYAN:  {
#if IS_WINDOWS()
			SetConsoleTextAttribute(ENGINE.console_handle, 11);
#else
			fprintf(stdout, "\033[0;36m");
#endif
		} break;
		
		case RDE_LOG_COLOR_WHITE:  {
#if IS_WINDOWS()
			SetConsoleTextAttribute(ENGINE.console_handle, 15);
#else
			fprintf(stdout, "\033[0;37m");
#endif
		} break;
	}

	va_list _args;
	va_start(_args, _fmt);
	vfprintf(stdout, _fmt, _args);
	va_end(_args);

#if IS_WINDOWS()
	SetConsoleTextAttribute(ENGINE.console_handle, 7);
#else
	fprintf(stdout, "\033[0m");
#endif
}

void rde_log_level_inner(RDE_LOG_LEVEL_ _level, const char* _fmt, ...) {
	switch(_level) {
		case RDE_LOG_LEVEL_ERROR: {
#if IS_WINDOWS()
			SetConsoleTextAttribute(ENGINE.console_handle, 12);
#else
			fprintf(stdout, "\033[0;31m");
#endif
			fprintf(stdout, "[ERROR] ");
		} break;
		
		case RDE_LOG_LEVEL_WARNING:  {
#if IS_WINDOWS()
			SetConsoleTextAttribute(ENGINE.console_handle, 6);
#else
			fprintf(stdout, "\033[0;33m");	
#endif
			fprintf(stdout, "[WARNING] ");
		} break;
		
		case RDE_LOG_LEVEL_DEBUG:  {
#if IS_WINDOWS()
			SetConsoleTextAttribute(ENGINE.console_handle, 11);
#else
			fprintf(stdout, "\033[0;36m");
#endif
			fprintf(stdout, "[DEBUG] ");
		} break;

		case RDE_LOG_LEVEL_INFO:  {
#if IS_WINDOWS()
			SetConsoleTextAttribute(ENGINE.console_handle, 7);
#endif
			fprintf(stdout, "[INFO] ");
		} break;
	}

	va_list _args;
	va_start(_args, _fmt);
	vfprintf(stdout, _fmt, _args);
	va_end(_args);
	
#if IS_WINDOWS()
	SetConsoleTextAttribute(ENGINE.console_handle, 7);
#else
	fprintf(stdout, "\033[0m");
#endif
}