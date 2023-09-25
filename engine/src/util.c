
const char* rde_util_get_file_name_extension(const char* _file_name) {
	const char* _dot = strrchr(_file_name, '.');
	if(!_dot || _dot == _file_name) return "";
	return _dot;
}

void rde_log_color(RDE_LOG_COLOR_ _color, const char* _fmt, ...) {
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
	fprintf(stdout, "\n");
	#else
	fprintf(stdout, "\033[0m \n");
	#endif
}

void rde_log_level(RDE_LOG_LEVEL_ _level, const char* _fmt, ...) {
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
	fprintf(stdout, "\n");
	#else
	fprintf(stdout, "\033[0m \n");
	#endif
}

size_t rde_util_get_string_width(const char* _string, const rde_font* _font) {
	int _text_size = strlen(_string);

	size_t _size = 0;
	for (int _i = 0; _i < _text_size; _i++) {
		int _key = (int)_string[_i];
		rde_font_char_info _char_info = _font->chars[_key];
		_size += (_char_info.advance.x >> 6); // /64.f
	}

	return _size;
}