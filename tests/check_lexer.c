#include <check.h>
#include <stdio.h>
#include "lexer/lexer.h"
#include "lexer/token.h"
#include "debug.h"
#include "utils.h"


/*
 * Helpers
 */

typedef struct {
	char *literal;
	char *diag;
	Token *expected;
} Test;

static void test_tokens(Lexer *lex, char *diag, Token *tokens);

static void test(Test test) {
	printf("- \e[2m\"%s\"\e[0m\n", test.literal);

	Lexer lex;
	ck_assert(lexer_init(test.literal, &lex) == 0);

	test_tokens(&lex, test.diag, test.expected);
}

static void test_many(Test *tests) {
	for (int i = 0; tests[i].literal != 0; i++)
		test(tests[i]);
}

/*
 * Testing
 */

static void test_tokens(Lexer *lex, char *diag, Token *tokens) {
	Token actual, expected;
	size_t expected_i = 0;

	do {
		actual = lex->lex(lex);
		expected = tokens[expected_i];

		ck_assert_msg(actual.kind == expected.kind, "Assert tokens' kind (%s and %s) failed on item %zu", token_strkind(actual.kind), token_strkind(expected.kind), expected_i);
		ck_assert_msg(actual.span.pos == expected.span.pos, "Assert tokens' pos (%zu; expected %zu) failed on item %zu", actual.span.pos, expected.span.pos, expected_i);
		ck_assert_msg(actual.span.len == expected.span.len, "Assert tokens' len (%zu; expected %zu) failed on item %zu", actual.span.len, expected.span.len, expected_i);

		expected_i++;
	}
	while (actual.kind != EOFTk && expected.kind != EOFTk);
}

/*
 * Tests
 */

START_TEST (test_symbols) {
	char *literal = "+-*/()=";
	Token expected[] = {
		new_token(PlusTk, literal, 0, 1),
		new_token(DashTk, literal, 1, 1),
		new_token(AsteriskTk, literal, 2, 1),
		new_token(SlashTk, literal, 3, 1),
		new_token(OpenParenTk, literal, 4, 1),
		new_token(CloseParenTk, literal, 5, 1),
		new_token(EqualsTk, literal, 6, 1),
		new_token(EOFTk, literal, 7, 1)
	};

	test((Test) {literal, NULL, expected});
}
END_TEST

START_TEST (test_numbers) {
	char *literal_1 = "0123456789";
	Token expected_1[] = {
		new_token(IntTk, literal_1, 0, 10),
		new_token(EOFTk, literal_1, 10, 1)
	};

	char *literal_2 = "123456789012345678890";
	Token expected_2[] = {
		new_token(IntTk, literal_2, 0, 21),
		new_token(EOFTk, literal_2, 21, 1)
	};

	char *literal_3 = "0";
	Token expected_3[] = {
		new_token(IntTk, literal_3, 0, 1),
		new_token(EOFTk, literal_3, 1, 1)
	};

	char *literal_4 = "1";
	Token expected_4[] = {
		new_token(IntTk, literal_4, 0, 1),
		new_token(EOFTk, literal_4, 1, 1)
	};

	char *literal_5 = "23";
	Token expected_5[] = {
		new_token(IntTk, literal_5, 0, 2),
		new_token(EOFTk, literal_5, 2, 1)
	};

	char *literal_6 = "8";
	Token expected_6[] = {
		new_token(IntTk, literal_6, 0, 1),
		new_token(EOFTk, literal_6, 1, 1)
	};

	char *literal_7 = ".123";
	Token expected_7[] = {
		new_token(FloatTk, literal_7, 0, 4),
		new_token(EOFTk, literal_7, 4, 1)
	};

	char *literal_8 = "123.456";
	Token expected_8[] = {
		new_token(FloatTk, literal_8, 0, 7),
		new_token(EOFTk, literal_8, 7, 1)
	};

	char *literal_9 = ".1.2";
	char *diag_9 = "A float cannot have more than one fractional part";
	Token expected_9[] = {
		new_token(FloatTk, literal_9, 0, 2),
		new_token(FloatTk, literal_9, 2, 2),
		new_token(EOFTk, literal_9, 4, 1)
	};

	Test tests[] = {
		{ literal_1, NULL, expected_1 },
		{ literal_2, NULL, expected_2 },
		{ literal_3, NULL, expected_3 },
		{ literal_4, NULL, expected_4 },
		{ literal_5, NULL, expected_5 },
		{ literal_6, NULL, expected_6 },
		{ literal_7, NULL, expected_7 },
		{ literal_8, NULL, expected_8 },
		{ literal_9, diag_9, expected_9 },
		{0}
	};

	test_many(tests);
}
END_TEST

START_TEST (test_identifiers) {
	char *literal_1 = "abc";
	Token expected_1[] = {
		new_token(IdentifierTk, literal_1, 0, 3),
		new_token(EOFTk, literal_1, 3, 1)
	};

	char *literal_2 = "a12";
	Token expected_2[] = {
		new_token(IdentifierTk, literal_2, 0, 3),
		new_token(EOFTk, literal_2, 3, 1)
	};

	char *literal_3 = "a__";
	Token expected_3[] = {
		new_token(IdentifierTk, literal_3, 0, 3),
		new_token(EOFTk, literal_3, 3, 1)
	};

	char *literal_4 = "a_34";
	Token expected_4[] = {
		new_token(IdentifierTk, literal_4, 0, 4),
		new_token(EOFTk, literal_4, 4, 1)
	};

	char *literal_5 = "a34_";
	Token expected_5[] = {
		new_token(IdentifierTk, literal_5, 0, 4),
		new_token(EOFTk, literal_5, 4, 1)
	};

	char *literal_6 = "_a3";
	Token expected_6[] = {
		new_token(IdentifierTk, literal_6, 0, 3),
		new_token(EOFTk, literal_6, 3, 1)
	};

	char *literal_7 = "_4v";
	Token expected_7[] = {
		new_token(IdentifierTk, literal_7, 0, 3),
		new_token(EOFTk, literal_7, 3, 1)
	};

	Test tests[] = {
		{literal_1, NULL, expected_1},
		{literal_2, NULL, expected_2},
		{literal_3, NULL, expected_3},
		{literal_4, NULL, expected_4},
		{literal_5, NULL, expected_5},
		{literal_6, NULL, expected_6},
		{literal_7, NULL, expected_7},
		{0}
	};

	test_many(tests);
}
END_TEST

/*
 * Main
 */

Suite *lexer_suite() {
	TCase *tc_core = tcase_create("Lexer core");

	tcase_add_test(tc_core, test_symbols);
	tcase_add_test(tc_core, test_numbers);
	tcase_add_test(tc_core, test_identifiers);

	Suite *s = suite_create("Lexer");
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
