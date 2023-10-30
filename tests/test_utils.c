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