#include <thatlang/lexer/lexer.h>
#include <thatlang/lexer/token.h>
#include <thatlang/diags/bag.h>
#include <intern/debug.h>
#include <intern/utils.h>

#include <check.h>
#include <stdio.h>
#include <unistd.h>


/*
 * Helpers
 */

#define STRVIEW(cstr) (strview_create((cstr), strlen((cstr))))

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

static void test_tokens(thLexer *lex, thToken *tokens, int test);

static void test(Test test, int testi) {
	print_literal(test.literal);

	int pipefd[2];
	pipe(pipefd);

	write(pipefd[1], test.literal, strlen(test.literal));
	close(pipefd[1]);

	thLexer lex;
	ck_assert(th_lexer_init(pipefd[0], &lex) == 0);

	test_tokens(&lex, test.expected, testi);
	close(pipefd[0]);
}

static void test_many(Test *tests) {
	for (size_t i = 0; tests[i].literal != 0; i++)
		test(tests[i], i + 1);
}

/*
 * Testing
 */

static void test_tokens(thLexer *lex, thToken *tokens, int test) {
	thToken actual, expected;
	size_t i = 0;
	thERR err;

	thTokenKind kind;
	char *literal;
	size_t pos, len;

	do {
		expected = tokens[i];
		err = lex->lex(lex, &actual);

		kind = actual.kind;
		literal = actual.literal;
		pos = actual.location.pos;
		len = actual.location.len;

		ck_assert_msg(err == 0, "Assert lex err isn't zero: test %d item %zu", test, i);
		ck_assert_msg(th_diags.diags == NULL, "Assert th_diags isn't NULL: test %d item %zu", test, i);
		ck_assert_msg(kind == expected.kind, "Assert tokens' kind (%s and %s) failed: test %d item %zu", token_strkind(kind), token_strkind(expected.kind), test, i);
		ck_assert_msg(pos == expected.location.pos, "Assert tokens' pos (%zu; expected %zu) failed: test %d item %zu", pos, expected.location.pos, test, i);
		ck_assert_msg(len == expected.location.len, "Assert tokens' len (%zu; expected %zu) failed: test %d item %zu", len, expected.location.len, test, i);

		if (literal == NULL || expected.literal == NULL)
			ck_assert_msg(
				literal == expected.literal,
				"Assert tokens' literal (\"%.*s\" and \"%.*s\") failed: test %d item %zu",

				literal == NULL          ? 0 : (int) len,                   literal == NULL          ? "" : literal,
				expected.literal == NULL ? 0 : (int) expected.location.len, expected.literal == NULL ? "" : expected.literal + expected.location.pos,
				test, i);

		else
			ck_assert_msg(
				strncmp(literal, expected.literal + expected.location.pos, expected.location.len) == 0,
				"Assert tokens' literal (\"%.*s\" and \"%.*s\") failed: test %d item %zu",
				(int) len, literal,
				(int) expected.location.len, expected.literal + expected.location.pos,
				test, i);

		i++;
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
		th_token_create(PlusTk, NULL, 0, 1),
		th_token_create(DashTk, NULL, 1, 1),
		th_token_create(SlashTk, NULL, 2, 1),
		th_token_create(CommaTk, NULL, 3, 1),
		th_token_create(OpenBraceTk, NULL, 4, 1),
		th_token_create(CloseBraceTk, NULL, 5, 1),
		th_token_create(OpenParenTk, NULL, 6, 1),
		th_token_create(CloseParenTk, NULL, 7, 1),
		th_token_create(LessThanTk, NULL, 8, 1),
		th_token_create(GreaterThanTk, NULL, 9, 1),
		th_token_create(AmpersandTk, NULL, 10, 1),
		th_token_create(AsteriskTk, NULL, 11, 1),
		th_token_create(PipeTk, NULL, 12, 1),
		th_token_create(EOFTk, NULL, 13, 1)
	};

	char literal_2[] = "===!==";
	thToken expected_2[] = {
		th_token_create(EqualsToTk, NULL, 0, 2),
		th_token_create(EqualsTk, NULL, 2, 1),
		th_token_create(DiffTk, NULL, 3, 2),
		th_token_create(EqualsTk, NULL, 5, 1),
		th_token_create(EOFTk, NULL, 6, 1)
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
		th_token_create(EOFTk, NULL, 10, 1)
	};

	char literal_2[] = "123456789012345678890";
	thToken expected_2[] = {
		th_token_create(IntTk, literal_2, 0, 21),
		th_token_create(EOFTk, NULL, 21, 1)
	};

	char literal_3[] = "0";
	thToken expected_3[] = {
		th_token_create(IntTk, literal_3, 0, 1),
		th_token_create(EOFTk, NULL, 1, 1)
	};

	char literal_4[] = "1";
	thToken expected_4[] = {
		th_token_create(IntTk, literal_4, 0, 1),
		th_token_create(EOFTk, NULL, 1, 1)
	};

	char literal_5[] = "23";
	thToken expected_5[] = {
		th_token_create(IntTk, literal_5, 0, 2),
		th_token_create(EOFTk, NULL, 2, 1)
	};

	char literal_6[] = "8";
	thToken expected_6[] = {
		th_token_create(IntTk, literal_6, 0, 1),
		th_token_create(EOFTk, NULL, 1, 1)
	};

	char literal_7[] = ".123";
	thToken expected_7[] = {
		th_token_create(UnknownTk, literal_7, 0, 1),
		th_token_create(IntTk, literal_7, 1, 3),
		th_token_create(EOFTk, NULL, 4, 1)
	};

	char literal_8[] = "123.456";
	thToken expected_8[] = {
		th_token_create(FloatTk, literal_8, 0, 7),
		th_token_create(EOFTk, NULL, 7, 1)
	};

	char literal_9[] = ".1.2";
	thToken expected_9[] = {
		th_token_create(UnknownTk, literal_9, 0, 1),
		th_token_create(FloatTk, literal_9, 1, 3),
		th_token_create(EOFTk, NULL, 4, 1)
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
		th_token_create(EOFTk, NULL, 3, 1)
	};

	char literal_2[] = "a12";
	thToken expected_2[] = {
		th_token_create(IdentifierTk, literal_2, 0, 3),
		th_token_create(EOFTk, NULL, 3, 1)
	};

	char literal_3[] = "a__";
	thToken expected_3[] = {
		th_token_create(IdentifierTk, literal_3, 0, 3),
		th_token_create(EOFTk, NULL, 3, 1)
	};
  
	char literal_4[] = "a_34";
	thToken expected_4[] = {
		th_token_create(IdentifierTk, literal_4, 0, 4),
		th_token_create(EOFTk, NULL, 4, 1)
	};

	char literal_5[] = "a34_";
	thToken expected_5[] = {
		th_token_create(IdentifierTk, literal_5, 0, 4),
		th_token_create(EOFTk, NULL, 4, 1)
	};

	char literal_6[] = "_a3";
	thToken expected_6[] = {
		th_token_create(IdentifierTk, literal_6, 0, 3),
		th_token_create(EOFTk, NULL, 3, 1)
	};

	char literal_7[] = "_4v";
	thToken expected_7[] = {
		th_token_create(IdentifierTk, literal_7, 0, 3),
		th_token_create(EOFTk, NULL, 3, 1)
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
		th_token_create(EOFTk, NULL, 6, 1)
	};

	char literal_2[] = "aimport";
	thToken expected_2[] = { 
		th_token_create(IdentifierTk, literal_2, 0, 7),
		th_token_create(EOFTk, NULL, 7, 1)
	};

	char literal_3[] = "importa";
	thToken expected_3[] = { 
		th_token_create(IdentifierTk, literal_3, 0, 7),
		th_token_create(EOFTk, NULL, 7, 1)
	};

	char literal_4[] = "pub";
	thToken expected_4[] = { 
		th_token_create(PubKw, literal_4, 0, 3),
		th_token_create(EOFTk, NULL, 3, 1)
	};

	char literal_5[] = "local";
	thToken expected_5[] = { 
		th_token_create(LocalKw, literal_5, 0, 5),
		th_token_create(EOFTk, NULL, 5, 1)
	};

	char literal_6[] = "type";
	thToken expected_6[] = { 
		th_token_create(TypeKw, literal_6, 0, 4),
		th_token_create(EOFTk, NULL, 4, 1)
	};

	char literal_7[] = "as";
	thToken expected_7[] = { 
		th_token_create(AsKw, literal_7, 0, 2),
		th_token_create(EOFTk, NULL, 2, 1)
	};

	char literal_8[] = "not";
	thToken expected_8[] = { 
		th_token_create(NotKw, literal_8, 0, 3),
		th_token_create(EOFTk, NULL, 3, 1)
	};

	char literal_9[] = "and";
	thToken expected_9[] = { 
		th_token_create(AndKw, literal_9, 0, 3),
		th_token_create(EOFTk, NULL, 3, 1)
	};

	char literal_10[] = "or";
	thToken expected_10[] = { 
		th_token_create(OrKw, literal_10, 0, 2),
		th_token_create(EOFTk, NULL, 2, 1)
	};

	char literal_11[] = "fn";
	thToken expected_11[] = { 
		th_token_create(FnKw, literal_11, 0, 2),
		th_token_create(EOFTk, NULL, 2, 1)
	};

	char literal_12[] = "return";
	thToken expected_12[] = { 
		th_token_create(ReturnKw, literal_12, 0, 6),
		th_token_create(EOFTk, NULL, 6, 1)
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
		th_token_create(EOFTk, NULL, 6, 1)
	};

	char literal_2[] = "- -- this is a dash token :)";
	thToken expected_2[] = { 
		th_token_create(DashTk, NULL, 0, 1),
		th_token_create(EOFTk, NULL, 28, 1)
	};

	char literal_3[] = "abc == 2 {\n-- Do something here idk\n}";
	thToken expected_3[] = { 
		th_token_create(IdentifierTk, literal_3, 0, 3),
		th_token_create(EqualsToTk, NULL, 4, 2),
		th_token_create(IntTk, literal_3, 7, 1),
		th_token_create(OpenBraceTk, NULL, 9, 1),
		th_token_create(CloseBraceTk, NULL, 36, 1),
		th_token_create(EOFTk, NULL, 37, 1)
	};

	char literal_4[] = "-- +-/,{}()<>===!=&*|";
	thToken expected_4[] = { 
		th_token_create(EOFTk, NULL, 21, 1)
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
		th_token_create(DashTk, NULL, 1, 1),
		th_token_create(EOFTk, NULL, 3, 1)
	};

	char literal_2[] = "\nab\n";
	thToken expected_2[] = {
		th_token_create(IdentifierTk, literal_2, 1, 2),
		th_token_create(EOFTk, NULL, 4, 1)
	};

	char literal_3[] = " 123 ";
	thToken expected_3[] = {
		th_token_create(IntTk, literal_3, 1, 3),
		th_token_create(EOFTk, NULL, 5, 1)
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
