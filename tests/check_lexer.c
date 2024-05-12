#include <thatlang/lexer/lexer.h>
#include <thatlang/lexer/token.h>
#include <intern/debug.h>
#include <intern/utils.h>

#include <check.h>
#include <stdio.h>


/*
 * Helpers
 */

typedef struct {
	char *literal;
	thToken *expected;
} Test;

static void print_literal(char *literal) {
	printf("- \e[2m\"");

	size_t i = 0;
	unsigned char ch = literal[i];

	for (; literal[i] != 0; ch = literal[++i]) {
		if      (ch == '\n') printf("\e[m\\n\e[2m");
		else if (ch == '\t') printf("\e[m\\t\e[2m");
		else    putchar(ch);
	}

	printf("\"\e[m\n");
}

static void test_tokens(thLexer *lex, thToken *tokens);

static void test(Test test) {
	print_literal(test.literal);

	thLexer lex;
	ck_assert(th_lexer_init(test.literal, &lex) == 0);

	test_tokens(&lex, test.expected);
}

static void test_many(Test *tests) {
	for (size_t i = 0; tests[i].literal != 0; i++)
		test(tests[i]);
}

/*
 * Testing
 */

static void test_tokens(thLexer *lex, thToken *tokens) {
	thToken actual, expected;
	size_t expected_i = 0;

	do {
		actual = lex->lex(lex);
		expected = tokens[expected_i];

		ck_assert_msg(actual.kind == expected.kind, "Assert tokens' kind (%s and %s) failed on item %zu", token_strkind(actual.kind), token_strkind(expected.kind), expected_i);
		ck_assert_msg(actual.location.pos == expected.location.pos, "Assert tokens' pos (%zu; expected %zu) failed on item %zu", actual.location.pos, expected.location.pos, expected_i);
		ck_assert_msg(actual.location.len == expected.location.len, "Assert tokens' len (%zu; expected %zu) failed on item %zu", actual.location.len, expected.location.len, expected_i);

		expected_i++;
	}
	while (actual.kind != EOFTk && expected.kind != EOFTk);
}

/*
 * Tests
 */

START_TEST (test_symbols) {
	puts("--- test_symbols ---");

	char literal_1[] = "+-/,{}()<>&*|";
	thToken expected_1[] = {
		th_token_create(PlusTk, literal_1, 0, 1),
		th_token_create(DashTk, literal_1, 1, 1),
		th_token_create(SlashTk, literal_1, 2, 1),
		th_token_create(CommaTk, literal_1, 3, 1),
		th_token_create(OpenBraceTk, literal_1, 4, 1),
		th_token_create(CloseBraceTk, literal_1, 5, 1),
		th_token_create(OpenParenTk, literal_1, 6, 1),
		th_token_create(CloseParenTk, literal_1, 7, 1),
		th_token_create(LessThanTk, literal_1, 8, 1),
		th_token_create(GreaterThanTk, literal_1, 9, 1),
		th_token_create(AmpersandTk, literal_1, 10, 1),
		th_token_create(AsteriskTk, literal_1, 11, 1),
		th_token_create(PipeTk, literal_1, 12, 1),
		th_token_create(EOFTk, literal_1, 13, 1)
	};

	char literal_2[] = "===!==";
	thToken expected_2[] = {
		th_token_create(EqualsToTk, literal_2, 0, 2),
		th_token_create(EqualsTk, literal_2, 2, 1),
		th_token_create(DiffTk, literal_2, 3, 2),
		th_token_create(EqualsTk, literal_2, 5, 1),
		th_token_create(EOFTk, literal_2, 6, 1)
	};

	Test tests[] = {
		{literal_1, expected_1},
		{literal_2, expected_2},
		{}
	};

	test_many(tests);
}
END_TEST

START_TEST (test_numbers) {
	puts("--- test_numbers ---");

	char literal_1[] = "0123456789";
	thToken expected_1[] = {
		th_token_create(IntTk, literal_1, 0, 10),
		th_token_create(EOFTk, literal_1, 10, 1)
	};

	char literal_2[] = "123456789012345678890";
	thToken expected_2[] = {
		th_token_create(IntTk, literal_2, 0, 21),
		th_token_create(EOFTk, literal_2, 21, 1)
	};

	char literal_3[] = "0";
	thToken expected_3[] = {
		th_token_create(IntTk, literal_3, 0, 1),
		th_token_create(EOFTk, literal_3, 1, 1)
	};

	char literal_4[] = "1";
	thToken expected_4[] = {
		th_token_create(IntTk, literal_4, 0, 1),
		th_token_create(EOFTk, literal_4, 1, 1)
	};

	char literal_5[] = "23";
	thToken expected_5[] = {
		th_token_create(IntTk, literal_5, 0, 2),
		th_token_create(EOFTk, literal_5, 2, 1)
	};

	char literal_6[] = "8";
	thToken expected_6[] = {
		th_token_create(IntTk, literal_6, 0, 1),
		th_token_create(EOFTk, literal_6, 1, 1)
	};

	char literal_7[] = ".123";
	thToken expected_7[] = {
		th_token_create(UnknownTk, literal_7, 0, 1),
		th_token_create(IntTk, literal_7, 1, 3),
		th_token_create(EOFTk, literal_7, 4, 1)
	};

	char literal_8[] = "123.456";
	thToken expected_8[] = {
		th_token_create(FloatTk, literal_8, 0, 7),
		th_token_create(EOFTk, literal_8, 7, 1)
	};

	char literal_9[] = ".1.2";
	thToken expected_9[] = {
		th_token_create(UnknownTk, literal_9, 0, 1),
		th_token_create(FloatTk, literal_9, 1, 3),
		th_token_create(EOFTk, literal_9, 4, 1)
	};

	Test tests[] = {
		{ literal_1, expected_1 },
		{ literal_2, expected_2 },
		{ literal_3, expected_3 },
		{ literal_4, expected_4 },
		{ literal_5, expected_5 },
		{ literal_6, expected_6 },
		{ literal_7, expected_7 },
		{ literal_8, expected_8 },
		{ literal_9, expected_9 },
		{}
	};

	test_many(tests);
}
END_TEST

START_TEST (test_identifiers) {
	puts("--- test_identifiers ---");

	char literal_1[] = "abc";
	thToken expected_1[] = {
		th_token_create(IdentifierTk, literal_1, 0, 3),
		th_token_create(EOFTk, literal_1, 3, 1)
	};

	char literal_2[] = "a12";
	thToken expected_2[] = {
		th_token_create(IdentifierTk, literal_2, 0, 3),
		th_token_create(EOFTk, literal_2, 3, 1)
	};

	char literal_3[] = "a__";
	thToken expected_3[] = {
		th_token_create(IdentifierTk, literal_3, 0, 3),
		th_token_create(EOFTk, literal_3, 3, 1)
	};

	char literal_4[] = "a_34";
	thToken expected_4[] = {
		th_token_create(IdentifierTk, literal_4, 0, 4),
		th_token_create(EOFTk, literal_4, 4, 1)
	};

	char literal_5[] = "a34_";
	thToken expected_5[] = {
		th_token_create(IdentifierTk, literal_5, 0, 4),
		th_token_create(EOFTk, literal_5, 4, 1)
	};

	char literal_6[] = "_a3";
	thToken expected_6[] = {
		th_token_create(IdentifierTk, literal_6, 0, 3),
		th_token_create(EOFTk, literal_6, 3, 1)
	};

	char literal_7[] = "_4v";
	thToken expected_7[] = {
		th_token_create(IdentifierTk, literal_7, 0, 3),
		th_token_create(EOFTk, literal_7, 3, 1)
	};

	Test tests[] = {
		{literal_1, expected_1},
		{literal_2, expected_2},
		{literal_3, expected_3},
		{literal_4, expected_4},
		{literal_5, expected_5},
		{literal_6, expected_6},
		{literal_7, expected_7},
		{}
	};

	test_many(tests);
}
END_TEST

START_TEST (test_keywords) {
	puts("--- test_keywords ---");

	char literal_1[] = "import";
	thToken expected_1[] = {
		th_token_create(ImportKw, literal_1, 0, 6),
		th_token_create(EOFTk, literal_1, 6, 1)
	};

	char literal_2[] = "aimport";
	thToken expected_2[] = {
		th_token_create(IdentifierTk, literal_2, 0, 7),
		th_token_create(EOFTk, literal_2, 7, 1)
	};

	char literal_3[] = "importa";
	thToken expected_3[] = {
		th_token_create(IdentifierTk, literal_3, 0, 7),
		th_token_create(EOFTk, literal_3, 7, 1)
	};

	char literal_4[] = "pub";
	thToken expected_4[] = {
		th_token_create(PubKw, literal_4, 0, 3),
		th_token_create(EOFTk, literal_4, 3, 1)
	};

	char literal_5[] = "local";
	thToken expected_5[] = {
		th_token_create(LocalKw, literal_5, 0, 5),
		th_token_create(EOFTk, literal_5, 5, 1)
	};

	char literal_6[] = "type";
	thToken expected_6[] = {
		th_token_create(TypeKw, literal_6, 0, 4),
		th_token_create(EOFTk, literal_6, 4, 1)
	};

	char literal_7[] = "as";
	thToken expected_7[] = {
		th_token_create(AsKw, literal_7, 0, 2),
		th_token_create(EOFTk, literal_7, 2, 1)
	};

	char literal_8[] = "not";
	thToken expected_8[] = {
		th_token_create(NotKw, literal_8, 0, 3),
		th_token_create(EOFTk, literal_8, 3, 1)
	};

	char literal_9[] = "and";
	thToken expected_9[] = {
		th_token_create(AndKw, literal_9, 0, 3),
		th_token_create(EOFTk, literal_9, 3, 1)
	};

	char literal_10[] = "or";
	thToken expected_10[] = {
		th_token_create(OrKw, literal_10, 0, 2),
		th_token_create(EOFTk, literal_10, 2, 1)
	};

	char literal_11[] = "fn";
	thToken expected_11[] = {
		th_token_create(FnKw, literal_11, 0, 2),
		th_token_create(EOFTk, literal_11, 2, 1)
	};

	char literal_12[] = "return";
	thToken expected_12[] = {
		th_token_create(ReturnKw, literal_12, 0, 6),
		th_token_create(EOFTk, literal_12, 6, 1)
	};

	Test tests[] = {
		{literal_1, expected_1},
		{literal_2, expected_2},
		{literal_3, expected_3},
		{literal_4, expected_4},
		{literal_5, expected_5},
		{literal_6, expected_6},
		{literal_7, expected_7},
		{literal_8, expected_8},
		{literal_9, expected_9},
		{literal_10, expected_10},
		{literal_11, expected_11},
		{literal_12, expected_12},
		{}
	};

	test_many(tests);
}
END_TEST

START_TEST (test_comments) {
	puts("--- test_comments ---");

	char literal_1[] = "-- abc";
	thToken expected_1[] = {
		th_token_create(EOFTk, literal_1, 6, 1)
	};

	char literal_2[] = "- -- this is a dash token :)";
	thToken expected_2[] = {
		th_token_create(DashTk, literal_2, 0, 1),
		th_token_create(EOFTk, literal_2, 28, 1)
	};

	char literal_3[] = "abc == 2 {\n-- Do something here idk\n}";
	thToken expected_3[] = {
		th_token_create(IdentifierTk, literal_3, 0, 3),
		th_token_create(EqualsToTk, literal_3, 4, 2),
		th_token_create(IntTk, literal_3, 7, 1),
		th_token_create(OpenBraceTk, literal_3, 9, 1),
		th_token_create(CloseBraceTk, literal_3, 36, 1),
		th_token_create(EOFTk, literal_2, 37, 1)
	};

	char literal_4[] = "-- +-/,{}()<>===!=&*|";
	thToken expected_4[] = {
		th_token_create(EOFTk, literal_2, 21, 1)
	};

	Test tests[] = {
		{literal_1, expected_1},
		{literal_2, expected_2},
		{literal_3, expected_3},
		{literal_4, expected_4},
		{}
	};

	test_many(tests);
}
END_TEST

START_TEST (test_whitespaces) {
	puts("--- test_whitespaces ---");

	char literal_1[] = "\t-\t";
	thToken expected_1[] = {
		th_token_create(DashTk, literal_1, 1, 1),
		th_token_create(EOFTk, literal_1, 3, 1)
	};

	char literal_2[] = "\nab\n";
	thToken expected_2[] = {
		th_token_create(IdentifierTk, literal_2, 1, 2),
		th_token_create(EOFTk, literal_2, 4, 1)
	};

	char literal_3[] = " 123 ";
	thToken expected_3[] = {
		th_token_create(IntTk, literal_3, 1, 3),
		th_token_create(EOFTk, literal_3, 5, 1)
	};

	Test tests[] = {
		{literal_1, expected_1},
		{literal_2, expected_2},
		{literal_3, expected_3},
		{}
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
	tcase_add_test(tc_core, test_keywords);
	tcase_add_test(tc_core, test_comments);
	tcase_add_test(tc_core, test_whitespaces);

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
