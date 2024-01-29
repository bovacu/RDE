void test_dyn_array_init() {
	rde_log_color(RDE_LOG_COLOR_GREEN, "%s", "--- DYN ARRAY TESTS ---");
}

typedef struct {
    int a;
    float b;
} rde_arr_type_0;

typedef struct {
    int a;
    rde_arr_type_0 b;
} rde_arr_type_1;

#undef RDE_ARR_DEFAULT_CAPACITY
#define RDE_ARR_DEFAULT_CAPACITY 50

rde_arr_decl(int);
rde_arr_decl(float);
rde_arr_decl(rde_arr_type_0);
rde_arr_decl(rde_arr_type_1);


void test_dyn_array_0() {
    rde_arr_int _arr_int = {0};
    rde_arr_init(&_arr_int);
    rde_arr_new(&_arr_int);
    for(int _i = 0; _i < RDE_ARR_DEFAULT_CAPACITY * 2; _i++) {
        rde_arr_add(&_arr_int, _i);
        rde_test_assert_no_msg(rde_arr_get_length(&_arr_int) == (ulong)(_i + 1), "test_dyn_array/test_dyn_array_0", "0: Length should be %d, it is %d\n", _i + 1, rde_arr_get_length(&_arr_int));
        int _value = 0;
        rde_arr_get_element(&_arr_int, _i, _value);
        rde_test_assert_no_msg(_value == _i, "test_dyn_array/test_dyn_array_0", "1: Value at %d should be %d, but was %d\n", _i, _i, _value);
    }
    
    int _indices_to_remove[10] = { 0, 19, 23, 25, 34, 51, 63, 71, 79, 89 };
    
    for(int _i = 0; _i < 10; _i++) {
        rde_arr_remove(&_arr_int, _indices_to_remove[_i]);
    }
    
    rde_test_assert_no_msg(rde_arr_get_length(&_arr_int) == (ulong)(RDE_ARR_DEFAULT_CAPACITY * 2 - 10), "test_dyn_array/test_dyn_array_0", "2: Length should be %d, it is %d\n", (RDE_ARR_DEFAULT_CAPACITY * 2 - 10), rde_arr_get_length(&_arr_int));
    int _last_value = 0;
    int _last_index = (RDE_ARR_DEFAULT_CAPACITY * 2 - 10) - 1;
    rde_arr_get_element(&_arr_int, _last_index, _last_value);
    rde_test_assert_no_msg(_last_value == (RDE_ARR_DEFAULT_CAPACITY * 2 - 1), "test_dyn_array/test_dyn_array_0", "3: Value at %d should be %d, but was %d\n", _last_index, _last_index, _last_value);
    
    int _counter = 1;
    for(int _i = 0; _i < (RDE_ARR_DEFAULT_CAPACITY * 2) - 10; _i++) {
        for(int _j = 0; _j < 10; _j++) {
            if(_indices_to_remove[_j] == _i) {
                int _value = 0;
                rde_arr_get_element(&_arr_int, _indices_to_remove[_j], _value);
                rde_test_assert_no_msg(_value == _i + _counter, "test_dyn_array/test_dyn_array_0", "4: Value at %d should be %d, but was %d\n", _i, _i + _counter, _value);
                _counter++;
                break;
            }
        }
    }
    
    rde_arr_add(&_arr_int, 1000);
    rde_arr_get_element(&_arr_int, rde_arr_get_length(&_arr_int) - 1, _last_value);
    rde_test_assert_no_msg(_last_value == 1000, "test_dyn_array/test_dyn_array_0", "5: Value at %d should be %d, but was %d\n", _last_index, 1000, _last_value);
    
    rde_arr_free(&_arr_int);
}

REGISTER_TEST_MODULE(test_dyn_array,
                     test_dyn_array_init,
                     test_dyn_array_0)
