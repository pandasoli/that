#include <intern/str.h>

#include <check.h>
#include <string.h>


/*
 * Helpers
 */

static void test(thStrBuilder *sb, char *data, size_t len, int i) {
	thERR err = strbuilder_create(data, len, sb);

	ck_assert_msg(err == 0, "Assert err (%d) isn't zero for sb%d", err, i);
	ck_assert_msg(sb->size == len, "Assert StrBuilder.size (%zu and %zu) isn't as expected in sb%d", sb->size, len, i);
	ck_assert_msg(sb->cap == len + 1, "Assert StrBuilder.cap (%zu and %zu) isn't size + 1 in sb%d", sb->cap, sb->size + 1, i);
	ck_assert_msg(sb->append != NULL, "Assert StrBuilder.append is NULL in sb%d", i);
	ck_assert_msg(sb->append_cstr != NULL, "Assert StrBuilder.append_cstr is NULL in sb%d", i);

	if (sb->data == NULL)
		ck_assert_msg(sb->data == NULL, "Assert StrBuilder.data isn't NULL in sb%d", i);
	else
		ck_assert_msg(
			strncmp(sb->data, data, len) == 0,
			"Assert StrBuilder.data (\"%.*s\" and \"%.*s\") isn't the expected in sb%d",
			(int) sb->size, sb->data,
			(int) len, data,
			i);
}

/*
 * Tests
 */

START_TEST (test1) {
	thERR err;

	thStrBuilder sb_1;
	test(&sb_1, "abc", 3, 2);

	err = sb_1.append(&sb_1, 'd');
	ck_assert_msg(err == 0, "Assert err (%d) isn't zero for sb2 after append", err);
	ck_assert_msg(sb_1.size == 4, "Assert StrBuilder.size (%zu and 4) isn't as expected in sb2 after append", sb_1.size);
	ck_assert_msg(sb_1.cap == 8, "Assert StrBuilder.cap (%zu and 8) isn't as expected in sb2 after append", sb_1.cap);
	ck_assert_msg(
		strncmp(sb_1.data, "abcd", 5) == 0,
		"Assert StrBuilder.data (\"%.*s\" and \"abcd\") isn't the expected in sb2 after append",
		(int) sb_1.size, sb_1.data);

	err = sb_1.append_cstr(&sb_1, "efg", 3);
	ck_assert_msg(err == 0, "Assert err (%d) isn't zero for sb2 after append_cstr", err);
	ck_assert_msg(sb_1.size == 7, "Assert StrBuilder.size (%zu and 7) isn't as expected in sb2 after append_cstr", sb_1.size);
	ck_assert_msg(sb_1.cap == 8, "Assert StrBuilder.cap (%zu and 8) isn't as expected in sb2 after append_cstr", sb_1.cap);
	ck_assert_msg(
		strncmp(sb_1.data, "abcdefg", 7) == 0,
		"Assert StrBuilder.data (\"%.*s\" and \"abcdefg\") isn't the expected in sb2 after append_cstr",
		(int) sb_1.size, sb_1.data);
	sb_1.free(&sb_1);

	thStrBuilder sb_2;
	test(&sb_2, "\0", 1, 3);
	sb_2.free(&sb_2);

	thStrBuilder sb_3;
	char literal_4 = 'a';
	test(&sb_3, &literal_4, 1, 4);
	sb_3.free(&sb_3);
}
END_TEST

/*
* Main
*/

Suite *lexer_suite() {
	TCase *tc_core = tcase_create("StrBuilder");

	tcase_add_test(tc_core, test1);

	Suite *s = suite_create("Str");
	suite_add_tcase(s, tc_core);

	return s;
}

int main() {
	Suite *s = lexer_suite();
	SRunner *sr = srunner_create(s);

	srunner_run_all(sr, CK_NORMAL);
	int err = srunner_ntests_failed(sr);
	srunner_free(sr);

	return err;
}
