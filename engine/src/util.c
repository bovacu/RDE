// replace substring is from https://github.com/ipserc/strrep/blob/master/string_replace/Source/strrep.c
char* rde_inner_append_str(char* _string, const char* _append) {
	char* _new_string = malloc(strlen(_string) + strlen(_append) + 1);

	sprintf(_new_string, "%s%s", _string, _append);
	free(_string);
	return _new_string;
}

char* rde_inner_strtokk(char* _string, const char* _strf) {
	static char* _ptr;
	static char* _ptr2;

	if (!*_strf) return _string;
	if (_string) _ptr = _string;
	else {
		if (!_ptr2) return _ptr2;
		_ptr = _ptr2 + strlen(_strf);
	}

	if (_ptr) {
		_ptr2 = strstr(_ptr, _strf);
		if (_ptr2) memset(_ptr2, 0, strlen(_strf));
	}
	return _ptr;
}




// ==============================================================================
// =									API										=
// ==============================================================================

#define RDE_IMPLEMENT_SAFE_ARR_ACCESS(_type)																								\
_type rde_arr_s_get_##_type(uint _index, _type* _arr, size_t _arr_size, char* _fmt, ...) {											\
	if(_index >= _arr_size) {																												\
		va_list _args;																														\
		va_start(_args, _fmt);																												\
		vfprintf(stdout, _fmt, _args);																										\
		va_end(_args);																														\
		if(!rde_engine_logs_supressed()) {																									\
			printf("\n");																													\
		}																																	\
	}																																		\
	rde_critical_error(_index >= _arr_size, "Index accessed '%u' is greater than array size '%llu' in '%s'.\n", _index, _arr_size);			\
	return _arr[_index];																													\
}
#define RDE_IMPLEMENT_SAFE_ARR_SET(_type)																									\
void rde_arr_s_set_##_type(uint _index, _type _value, _type* _arr, size_t _arr_size, char* _fmt, ...) {								\
	if(_index >= _arr_size) {																												\
		va_list _args;																														\
		va_start(_args, _fmt);																												\
		vfprintf(stdout, _fmt, _args);																										\
		va_end(_args);																														\
		if(!rde_engine_logs_supressed()) {																									\
			printf("\n");																													\
		}																																	\
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

void rde_util_file_sanitaize_path(const char* _path, char* _sanitized, size_t _sanitized_size) {
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

char* rde_util_string_replace_substring(char* _string, char* _old_string, char* _new_string, int* _output_appearences) {
	char* _str;
	char* _ptr;
	char* _strrep;

	_str = (char *)malloc(strlen(_string) + 1);
	sprintf(_str, "%s", _string);
	if (!*_old_string) return _str;
	_ptr = rde_inner_strtokk(_str, _old_string);
	_strrep = malloc(strlen(_ptr) + 1);
	memset(_strrep, 0, strlen(_ptr) + 1);
	while (_ptr) {
		_strrep = rde_inner_append_str(_strrep, _ptr);
		_ptr = rde_inner_strtokk(NULL, _old_string);
		if (_ptr) _strrep = rde_inner_append_str(_strrep, _new_string);
		if(_ptr && _output_appearences != NULL) {
			(*_output_appearences)++;
		}
	}

	free(_str);
	return _strrep;
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
	char* _string_ptr = rde_strtok(_string, _split_mark, NULL);

	while(_string_ptr != NULL) {
		rde_strcat(_split_array[_amount], strlen(_string_ptr), _string_ptr);
		_string_ptr = rde_strtok(NULL, _split_mark, NULL);
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