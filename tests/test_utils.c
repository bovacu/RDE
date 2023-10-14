#include <stdio.h>

typedef void(*test_func_ptr)(void);
test_func_ptr* test_functions = NULL;

#if IS_WINDOWS()
HANDLE console_handle = NULL;
#endif

void init_test_config() {
#if IS_WINDOWS()
	console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
}

void rde_test_assert(bool _ok, const char* _test_name, const char* _fmt, ...) {
	if(!_ok) {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "\tTEST FAILED: %s", _test_name);
		va_list _args;
		va_start(_args, _fmt);
		vfprintf(stdout, _fmt, _args);
		va_end(_args);
		exit(-1);
	}

	rde_log_color(RDE_LOG_COLOR_GREEN, "\tTEST %s PASSED", _test_name);
}

static void rde_register_function(test_func_ptr _fn) {
	stbds_arrput(test_functions, _fn);
}

#define REGISTER_TEST_MODULE(_test, ...) 											\
	void _test() {																	\
		test_func_ptr __varlist[] = {__VA_ARGS__};									\
		for(size_t _i = 0; _i < sizeof(__varlist) / sizeof(test_func_ptr); _i++) {	\
			rde_register_function(__varlist[_i]);									\
		}																			\
	}

#define RUN_ALL_TESTS(...) 														\
	test_func_ptr __varlist[] = {__VA_ARGS__};									\
	for(size_t _i = 0; _i < sizeof(__varlist) / sizeof(test_func_ptr); _i++) {	\
		__varlist[_i]();														\
	}																			\
																				\
	for(size_t _i = 0; _i < stbds_arrlenu(test_functions); _i++) {				\
		printf("\n");															\
		test_functions[_i]();													\
	}																			\
	stbds_arrfree(test_functions);

	size_t rde_util_string_split(char* _string, char** _split_array, const char* _split_mark) {
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

	void rde_log_color(RDE_LOG_COLOR_ _color, const char* _fmt, ...) {
		switch(_color) {
			case RDE_LOG_COLOR_RED: {
#if IS_WINDOWS()
				SetConsoleTextAttribute(console_handle, 12);
#else
				fprintf(stdout, "\033[0;31m");
#endif
			} break;
		
			case RDE_LOG_COLOR_GREEN:  {
#if IS_WINDOWS()
				SetConsoleTextAttribute(console_handle, 10);
#else
				fprintf(stdout, "\033[0;32m");	
#endif
			} break;
		
			case RDE_LOG_COLOR_YELLOW:  {
#if IS_WINDOWS()
				SetConsoleTextAttribute(console_handle, 6);
#else
				fprintf(stdout, "\033[0;33m");	
#endif
			} break;
		
			case RDE_LOG_COLOR_BLUE:  {
#if IS_WINDOWS()
				SetConsoleTextAttribute(console_handle, 9);
#else
				fprintf(stdout, "\033[0;34m");		
#endif
			} break;
		
			case RDE_LOG_COLOR_PURPLE:  {
#if IS_WINDOWS()
				SetConsoleTextAttribute(console_handle, 5);
#else
				fprintf(stdout, "\033[0;35m");
#endif
			} break;
		
			case RDE_LOG_COLOR_CYAN:  {
#if IS_WINDOWS()
				SetConsoleTextAttribute(console_handle, 11);
#else
				fprintf(stdout, "\033[0;36m");
#endif
			} break;
		
			case RDE_LOG_COLOR_WHITE:  {
#if IS_WINDOWS()
				SetConsoleTextAttribute(console_handle, 15);
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
		SetConsoleTextAttribute(console_handle, 7);
		fprintf(stdout, "\n");
#else
		fprintf(stdout, "\033[0m \n");
#endif
	}

	void rde_log_level(RDE_LOG_LEVEL_ _level, const char* _fmt, ...) {
		switch(_level) {
			case RDE_LOG_LEVEL_ERROR: {
#if IS_WINDOWS()
				SetConsoleTextAttribute(console_handle, 12);
#else
				fprintf(stdout, "\033[0;31m");
#endif
				fprintf(stdout, "[ERROR] ");
			} break;
		
			case RDE_LOG_LEVEL_WARNING:  {
#if IS_WINDOWS()
				SetConsoleTextAttribute(console_handle, 6);
#else
				fprintf(stdout, "\033[0;33m");	
#endif
				fprintf(stdout, "[WARNING] ");
			} break;
		
			case RDE_LOG_LEVEL_DEBUG:  {
#if IS_WINDOWS()
				SetConsoleTextAttribute(console_handle, 11);
#else
				fprintf(stdout, "\033[0;36m");
#endif
				fprintf(stdout, "[DEBUG] ");
			} break;

			case RDE_LOG_LEVEL_INFO:  {
#if IS_WINDOWS()
				SetConsoleTextAttribute(console_handle, 7);
#endif
				fprintf(stdout, "[INFO] ");
			} break;
		}

		va_list _args;
		va_start(_args, _fmt);
		vfprintf(stdout, _fmt, _args);
		va_end(_args);
	
#if IS_WINDOWS()
		SetConsoleTextAttribute(console_handle, 7);
		fprintf(stdout, "\n");
#else
		fprintf(stdout, "\033[0m \n");
#endif
	}