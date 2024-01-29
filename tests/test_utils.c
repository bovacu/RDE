#include <stdio.h>

typedef void(*test_func_ptr)(void);
test_func_ptr* test_functions = NULL;

#if RDE_IS_WINDOWS()
HANDLE console_handle = NULL;
#endif

void init_test_config() {
#if RDE_IS_WINDOWS()
	console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
}

void rde_test_assert_no_msg(bool _ok, const char* _test_name, const char* _fmt, ...) {
	if(!_ok) {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "\tTEST FAILED: %s", _test_name);
		va_list _args;
		va_start(_args, _fmt);
		vfprintf(stdout, _fmt, _args);
		va_end(_args);
		exit(-1);
	}
}
#define rde_test_assert(_ok, _test_name, _fmt, ...) rde_test_assert_no_msg(_ok, _test_name, _fmt, __VA_ARGS__); rde_log_color(RDE_LOG_COLOR_GREEN, "\tTEST %s PASSED", _test_name);

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
