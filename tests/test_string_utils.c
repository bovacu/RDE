void test_string_init() {
	rde_log_color(RDE_LOG_COLOR_GREEN, "--- UTIL_STRING TESTS ---");
}

void test_string_trim() {
	char _test_0[256] = " trim left";
	char _expected_0[256] = "trim left";
	char* _trimed_test_0 = rde_util_string_trim(_test_0);
	rde_test_assert(strcmp(_trimed_test_0, _expected_0) == 0, "test_string_trim/trim_left", "\tExpected: '%s', got '%s'", _expected_0, _trimed_test_0);

	char _test_1[256] = "trim right ";
	char _expected_1[256] = "trim right";
	char* _trimed_test_1 = rde_util_string_trim(_test_1);
	rde_test_assert(strcmp(_trimed_test_1, _expected_1) == 0, "test_string_trim/trim_right", "\tExpected: '%s', got '%s'", _expected_1, _trimed_test_1);

	char _test_2[256] = " trim both ";
	char _expected_2[256] = "trim both";
	char* _trimed_test_2 = rde_util_string_trim(_test_2);
	rde_test_assert(strcmp(_trimed_test_2, _expected_2) == 0, "test_string_trim/trim_both", "\tExpected: '%s', got '%s'", _expected_2, _trimed_test_2);

	char _test_3[256] = "        trim multi   ";
	char _expected_3[256] = "trim multi";
	char* _trimed_test_3 = rde_util_string_trim(_test_3);
	rde_test_assert(strcmp(_trimed_test_3, _expected_3) == 0, "test_string_trim/trim_multi", "\tExpected: '%s', got '%s'", _expected_3, _trimed_test_3);

	char _test_4[256] = "   \n\n   trim nl   \n";
	char _expected_4[256] = "trim nl";
	char* _trimed_test_4 = rde_util_string_trim(_test_4);
	rde_test_assert(strcmp(_trimed_test_4, _expected_4) == 0, "test_string_trim/trim_nl", "\tExpected: '%s', got '%s'", _expected_4, _trimed_test_4);

	char _test_5[256] = "   \r\n   trim nl linux  \r\n";
	char _expected_5[256] = "trim nl linux";
	char* _trimed_test_5 = rde_util_string_trim(_test_5);
	rde_test_assert(strcmp(_trimed_test_5, _expected_5) == 0, "test_string_trim/trim_nl_linux", "\tExpected: '%s', got '%s'", _expected_5, _trimed_test_5);
}

void test_string_starts_with() {
	char _test_0[256] = "My string";
	char _expected_0[256] = "My";
	rde_test_assert(rde_util_string_starts_with(_test_0, _expected_0), "test_string_starts_with/starts_yes", "\t'%s' does not start with '%s'", _test_0, _expected_0);

	char _test_1[256] = "My string";
	char _expected_1[256] = " My";
	rde_test_assert(!rde_util_string_starts_with(_test_1, _expected_1), "test_string_starts_with/starts_no", "\t'%s' does start with '%s'", _test_1, _expected_1);

	char _test_2[256] = "My string";
	char _expected_2[256] = " My stringextra";
	rde_test_assert(!rde_util_string_starts_with(_test_2, _expected_2), "test_string_starts_with/starts_bigger_than_string", "\t'%s' does start with '%s'", _test_2, _expected_2);

	char _test_3[256] = "My string";
	rde_test_assert(!rde_util_string_starts_with(_test_3, NULL), "test_string_starts_with/starts_prefix_NULL", "\t'%s' does start with '%s'", _test_3, NULL);

	char _expected_4[256] = "My";
	rde_test_assert(!rde_util_string_starts_with(NULL, _expected_4), "test_string_starts_with/starts_string_NULL", "\t'%s' does start with '%s'", NULL, _expected_4);
}

void test_string_ends_with() {
	char _test_0[256] = "My string";
	char _expected_0[256] = "ing";
	rde_test_assert(rde_util_string_ends_with(_test_0, _expected_0), "test_string_ends_with/ends_yes", "\t'%s' does not end with '%s'", _test_0, _expected_0);

	char _test_1[256] = "My string";
	char _expected_1[256] = "ing ";
	rde_test_assert(!rde_util_string_ends_with(_test_1, _expected_1), "test_string_ends_with/ends_no", "\t'%s' does end with '%s'", _test_1, _expected_1);

	char _test_2[256] = "My string";
	char _expected_2[256] = " My stringextra";
	rde_test_assert(!rde_util_string_ends_with(_test_2, _expected_2), "test_string_ends_with/ends_bigger_than_string", "\t'%s' does start with '%s'", _test_2, _expected_2);

	char _test_3[256] = "My string";
	rde_test_assert(!rde_util_string_ends_with(_test_3, NULL), "test_string_ends_with/ends_prefix_NULL", "\t'%s' does start with '%s'", _test_3, NULL);

	char _expected_4[256] = "My";
	rde_test_assert(!rde_util_string_ends_with(NULL, _expected_4), "test_string_ends_with/ends_string_NULL", "\t'%s' does start with '%s'", NULL, _expected_4);
}

void test_string_containns_substring() {
	char _test_string[256] = "This is my big string with @lso s0me non \"standard\" (char$)";
	char _expected_0[256] = "@lso";
	rde_test_assert(rde_util_string_contains_substring(_test_string, _expected_0, false), "test_string_containns_substring/contains_0_c_insensitive", "\t'%s' does not contain '%s'", _test_string, _expected_0);

	char _expected_1[256] = "This Is";
	rde_test_assert(rde_util_string_contains_substring(_test_string, _expected_1, false), "test_string_containns_substring/contains_1_c_insensitive", "\t'%s' does not contain '%s'", _test_string, _expected_1);

	char _expected_2[256] = "@Lso";
	rde_test_assert(rde_util_string_contains_substring(_test_string, _expected_2, false), "test_string_containns_substring/contains_2_c_insensitive", "\t'%s' does not contain '%s'", _test_string, _expected_2);

	char _expected_3[256] = "with @lso sOme";
	rde_test_assert(!rde_util_string_contains_substring(_test_string, _expected_3, false), "test_string_containns_substring/no_contains_0_c_insensitive", "\t'%s' does contain '%s'", _test_string, _expected_3);

	char _expected_4[256] = "This is my b";
	rde_test_assert(rde_util_string_contains_substring(_test_string, _expected_4, true), "test_string_containns_substring/contains_0_c_sensitive", "\t'%s' does not contain '%s'", _test_string, _expected_4);

	char _expected_5[256] = "n \"stan";
	rde_test_assert(rde_util_string_contains_substring(_test_string, _expected_5, true), "test_string_containns_substring/contains_1_c_sensitive", "\t'%s' does not contain '%s'", _test_string, _expected_5);

	char _expected_6[256] = "n \"staN";
	rde_test_assert(!rde_util_string_contains_substring(_test_string, _expected_6, true), "test_string_containns_substring/no_contains_0_c_sensitive", "\t'%s' does not contain '%s'", _test_string, _expected_6);
}

void test_string_char_appearence() {
	char _test_string[256] = "Today'$ date is 20/10/2047 and it is an 'important date' becau$e testing is needed";
	char _seeking_char = '$';

	size_t _expected_amount_0 = 2;
	rde_test_assert(rde_util_string_char_appearences(_test_string, _seeking_char) == _expected_amount_0, "test_string_char_appearence/count_0", "\t'%s' does not contain '%c' %lu times", _test_string, _seeking_char, _expected_amount_0);

	_seeking_char = 'a';
	size_t _expected_amount_1 = 7;
	rde_test_assert(rde_util_string_char_appearences(_test_string, _seeking_char) == _expected_amount_1, "test_string_char_appearence/count_1", "\t'%s' does not contain '%c' %lu times", _test_string, _seeking_char, _expected_amount_1);

	_seeking_char = '@';
	size_t _expected_amount_2 = 0;
	rde_test_assert(rde_util_string_char_appearences(_test_string, _seeking_char) == _expected_amount_2, "test_string_char_appearence/count_2", "\t'%s' does not contain '%c' %lu times", _test_string, _seeking_char, _expected_amount_2);
}

void test_string_concat() {
	
}

REGISTER_TEST_MODULE(test_string_utils,
                     test_string_init,
                     test_string_trim,
                     test_string_starts_with,
                     test_string_ends_with,
                     test_string_containns_substring,
                     test_string_char_appearence,
                     test_string_concat)