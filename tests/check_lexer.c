#include "intern/str.h"
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
	thStrView literal;
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
	print_literal(test.literal.data);

	int pipefd[2];
	pipe(pipefd);

	write(pipefd[1], test.literal.data, strlen(test.literal.data));
	close(pipefd[1]);

	thLexer lex;
	ck_assert(th_lexer_init(pipefd[0], &lex) == 0);

	test_tokens(&lex, test.expected);
	close(pipefd[0]);
}

static void test_many(Test *tests) {
	for (size_t i = 0; tests[i].literal.data != 0; i++)
		test(tests[i]);
}

/*
 * Testing
 */

static void test_tokens(thLexer *lex, thToken *tokens) {
	thToken actual, expected;
	size_t expected_i = 0;
	thERR err;

	do {
		expected = tokens[expected_i];
		err = lex->lex(lex, &actual);

		ck_assert_msg(err == 0, "Assert lex err isn't zero on item %zu", expected_i);
		ck_assert_msg(th_diags.diags == NULL, "Assert th_diags isn't NULL on item %zu", expected_i);
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

	thStrView literal_1 = STRVIEW("+-/,{}()<>&*|");
	thToken expected_1[] = {
		th_token_create(PlusTk, &literal_1, 0, 1),
		th_token_create(DashTk, &literal_1, 1, 1),
		th_token_create(SlashTk, &literal_1, 2, 1),
		th_token_create(CommaTk, &literal_1, 3, 1),
		th_token_create(OpenBraceTk, &literal_1, 4, 1),
		th_token_create(CloseBraceTk, &literal_1, 5, 1),
		th_token_create(OpenParenTk, &literal_1, 6, 1),
		th_token_create(CloseParenTk, &literal_1, 7, 1),
		th_token_create(LessThanTk, &literal_1, 8, 1),
		th_token_create(GreaterThanTk, &literal_1, 9, 1),
		th_token_create(AmpersandTk, &literal_1, 10, 1),
		th_token_create(AsteriskTk, &literal_1, 11, 1),
		th_token_create(PipeTk, &literal_1, 12, 1),
		th_token_create(EOFTk, &literal_1, 13, 1)
	};

	thStrView literal_2 = STRVIEW("===!==");
	thToken expected_2[] = {
		th_token_create(EqualsToTk, &literal_2, 0, 2),
		th_token_create(EqualsTk, &literal_2, 2, 1),
		th_token_create(DiffTk, &literal_2, 3, 2),
		th_token_create(EqualsTk, &literal_2, 5, 1),
		th_token_create(EOFTk, &literal_2, 6, 1)
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

	thStrView literal_1 = STRVIEW("0123456789");
	thToken expected_1[] = {
		th_token_create(IntTk, &literal_1, 0, 10),
		th_token_create(EOFTk, &literal_1, 10, 1)
	};

	thStrView literal_2 = STRVIEW("123456789012345678890");
	thToken expected_2[] = {
		th_token_create(IntTk, &literal_2, 0, 21),
		th_token_create(EOFTk, &literal_2, 21, 1)
	};

	thStrView literal_3 = STRVIEW("0");
	thToken expected_3[] = {
		th_token_create(IntTk, &literal_3, 0, 1),
		th_token_create(EOFTk, &literal_3, 1, 1)
	};

	thStrView literal_4 = STRVIEW("1");
	thToken expected_4[] = {
		th_token_create(IntTk, &literal_4, 0, 1),
		th_token_create(EOFTk, &literal_4, 1, 1)
	};

	thStrView literal_5 = STRVIEW("23");
	thToken expected_5[] = {
		th_token_create(IntTk, &literal_5, 0, 2),
		th_token_create(EOFTk, &literal_5, 2, 1)
	};

	thStrView literal_6 = STRVIEW("8");
	thToken expected_6[] = {
		th_token_create(IntTk, &literal_6, 0, 1),
		th_token_create(EOFTk, &literal_6, 1, 1)
	};

	thStrView literal_7 = STRVIEW(".123");
	thToken expected_7[] = {
		th_token_create(UnknownTk, &literal_7, 0, 1),
		th_token_create(IntTk, &literal_7, 1, 3),
		th_token_create(EOFTk, &literal_7, 4, 1)
	};

	thStrView literal_8 = STRVIEW("123.456");
	thToken expected_8[] = {
		th_token_create(FloatTk, &literal_8, 0, 7),
		th_token_create(EOFTk, &literal_8, 7, 1)
	};

	thStrView literal_9 = STRVIEW(".1.2");
	thToken expected_9[] = {
		th_token_create(UnknownTk, &literal_9, 0, 1),
		th_token_create(FloatTk, &literal_9, 1, 3),
		th_token_create(EOFTk, &literal_9, 4, 1)
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

	thStrView literal_1 = STRVIEW("abc");
	thToken expected_1[] = {
		th_token_create(IdentifierTk, &literal_1, 0, 3),
		th_token_create(EOFTk, &literal_1, 3, 1)
	};

	thStrView literal_2 = STRVIEW("a12");
	thToken expected_2[] = {
		th_token_create(IdentifierTk, &literal_2, 0, 3),
		th_token_create(EOFTk, &literal_2, 3, 1)
	};

	thStrView literal_3 = STRVIEW("a__");
	thToken expected_3[] = {
		th_token_create(IdentifierTk, &literal_3, 0, 3),
		th_token_create(EOFTk, &literal_3, 3, 1)
	};
  
	thStrView literal_4 = STRVIEW("a_34");
	thToken expected_4[] = {
		th_token_create(IdentifierTk, &literal_4, 0, 4),
		th_token_create(EOFTk, &literal_4, 4, 1)
	};

	thStrView literal_5 = STRVIEW("a34_");
	thToken expected_5[] = {
		th_token_create(IdentifierTk, &literal_5, 0, 4),
		th_token_create(EOFTk, &literal_5, 4, 1)
	};

	thStrView literal_6 = STRVIEW("_a3");
	thToken expected_6[] = {
		th_token_create(IdentifierTk, &literal_6, 0, 3),
		th_token_create(EOFTk, &literal_6, 3, 1)
	};

	thStrView literal_7 = STRVIEW("_4v");
	thToken expected_7[] = {
		th_token_create(IdentifierTk, &literal_7, 0, 3),
		th_token_create(EOFTk, &literal_7, 3, 1)
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

	thStrView literal_1 = STRVIEW("import");
	thToken expected_1[] = { 
		th_token_create(ImportKw, &literal_1, 0, 6),
		th_token_create(EOFTk, &literal_1, 6, 1)
	};

	thStrView literal_2 = STRVIEW("aimport");
	thToken expected_2[] = { 
		th_token_create(IdentifierTk, &literal_2, 0, 7),
		th_token_create(EOFTk, &literal_2, 7, 1)
	};

	thStrView literal_3 = STRVIEW("importa");
	thToken expected_3[] = { 
		th_token_create(IdentifierTk, &literal_3, 0, 7),
		th_token_create(EOFTk, &literal_3, 7, 1)
	};

	thStrView literal_4 = STRVIEW("pub");
	thToken expected_4[] = { 
		th_token_create(PubKw, &literal_4, 0, 3),
		th_token_create(EOFTk, &literal_4, 3, 1)
	};

	thStrView literal_5 = STRVIEW("local");
	thToken expected_5[] = { 
		th_token_create(LocalKw, &literal_5, 0, 5),
		th_token_create(EOFTk, &literal_5, 5, 1)
	};

	thStrView literal_6 = STRVIEW("type");
	thToken expected_6[] = { 
		th_token_create(TypeKw, &literal_6, 0, 4),
		th_token_create(EOFTk, &literal_6, 4, 1)
	};

	thStrView literal_7 = STRVIEW("as");
	thToken expected_7[] = { 
		th_token_create(AsKw, &literal_7, 0, 2),
		th_token_create(EOFTk, &literal_7, 2, 1)
	};

	thStrView literal_8 = STRVIEW("not");
	thToken expected_8[] = { 
		th_token_create(NotKw, &literal_8, 0, 3),
		th_token_create(EOFTk, &literal_8, 3, 1)
	};

	thStrView literal_9 = STRVIEW("and");
	thToken expected_9[] = { 
		th_token_create(AndKw, &literal_9, 0, 3),
		th_token_create(EOFTk, &literal_9, 3, 1)
	};

	thStrView literal_10 = STRVIEW("or");
	thToken expected_10[] = { 
		th_token_create(OrKw, &literal_10, 0, 2),
		th_token_create(EOFTk, &literal_10, 2, 1)
	};

	thStrView literal_11 = STRVIEW("fn");
	thToken expected_11[] = { 
		th_token_create(FnKw, &literal_11, 0, 2),
		th_token_create(EOFTk, &literal_11, 2, 1)
	};

	thStrView literal_12 = STRVIEW("return");
	thToken expected_12[] = { 
		th_token_create(ReturnKw, &literal_12, 0, 6),
		th_token_create(EOFTk, &literal_12, 6, 1)
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

	thStrView literal_1 = STRVIEW("-- abc");
	thToken expected_1[] = { 
		th_token_create(EOFTk, &literal_1, 6, 1)
	};

	thStrView literal_2 = STRVIEW("- -- this is a dash token :)");
	thToken expected_2[] = { 
		th_token_create(DashTk, &literal_2, 0, 1),
		th_token_create(EOFTk, &literal_2, 28, 1)
	};

	thStrView literal_3 = STRVIEW("abc == 2 {\n-- Do something here idk\n}");
	thToken expected_3[] = { 
		th_token_create(IdentifierTk, &literal_3, 0, 3),
		th_token_create(EqualsToTk, &literal_3, 4, 2),
		th_token_create(IntTk, &literal_3, 7, 1),
		th_token_create(OpenBraceTk, &literal_3, 9, 1),
		th_token_create(CloseBraceTk, &literal_3, 36, 1),
		th_token_create(EOFTk, &literal_2, 37, 1)
	};

	thStrView literal_4 = STRVIEW("-- +-/,{}()<>===!=&*|");
	thToken expected_4[] = { 
		th_token_create(EOFTk, &literal_2, 21, 1)
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

	thStrView literal_1 = STRVIEW("\t-\t");
	thToken expected_1[] = {
		th_token_create(DashTk, &literal_1, 1, 1),
		th_token_create(EOFTk, &literal_1, 3, 1)
	};

	thStrView literal_2 = STRVIEW("\nab\n");
	thToken expected_2[] = {
		th_token_create(IdentifierTk, &literal_2, 1, 2),
		th_token_create(EOFTk, &literal_2, 4, 1)
	};

	thStrView literal_3 = STRVIEW(" 123 ");
	thToken expected_3[] = {
		th_token_create(IntTk, &literal_3, 1, 3),
		th_token_create(EOFTk, &literal_3, 5, 1)
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
