#include <thatlang/lexer/lexer.h>
#include <thatlang/lexer/token.h>
#include <thatlang/diags/bag.h>
#include <intern/debug.h>
#include <intern/utils.h>

#include <check.h>
#include <stdio.h>
#include <unistd.h>


/*
 * Testing
 */

static int testi = 0;

static void test_tokens(thLexer *lex, thToken *tokens) {
	thToken actual, expected;
	size_t i = 0;
	thERR err;

	thTokenKind kind;
	const char *literal;
	size_t pos, len;

	do {
		expected = tokens[i];
		err = lex->lex(lex, &actual);

		kind = actual.kind;
		literal = actual.literal;
		pos = actual.location.pos;
		len = actual.location.len;

		ck_assert_msg(err == 0, "Assert lex err isn't zero: test %d item %zu", testi, i);
		ck_assert_msg(th_diags.diags == NULL, "Assert th_diags isn't NULL: test %d item %zu", testi, i);
		ck_assert_msg(kind == expected.kind, "Assert tokens' kind (%s and %s) failed: test %d item %zu", token_strkind(kind), token_strkind(expected.kind), testi, i);
		ck_assert_msg(pos == expected.location.pos, "Assert tokens' pos (%zu; expected %zu) failed: test %d item %zu", pos, expected.location.pos, testi, i);
		ck_assert_msg(len == expected.location.len, "Assert tokens' len (%zu; expected %zu) failed: test %d item %zu", len, expected.location.len, testi, i);

		if (literal == NULL || expected.literal == NULL)
			ck_assert_msg(
				literal == expected.literal,
				"Assert tokens' literal (\"%.*s\" and \"%.*s\") failed: test %d item %zu",

				literal == NULL          ? 0 : (int) len,                   literal == NULL          ? "" : literal,
				expected.literal == NULL ? 0 : (int) expected.location.len, expected.literal == NULL ? "" : expected.literal + expected.location.pos,
				testi, i);

		else
			ck_assert_msg(
				strncmp(literal, expected.literal + expected.location.pos, expected.location.len) == 0,
				"Assert tokens' literal (\"%.*s\" and \"%.*s\") failed: test %d item %zu",
				(int) len, literal,
				(int) expected.location.len, expected.literal + expected.location.pos,
				testi, i);

		i++;
	}
	while (actual.kind != thEOITk && expected.kind != thEOITk);
}

/*
 * Helpers
 */

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

static void test(char *literal, thToken *expected) {
	print_literal(literal);

	int pipefd[2];
	pipe(pipefd);

	write(pipefd[1], literal, strlen(literal));
	close(pipefd[1]);

	thLexer lex;
	ck_assert(th_lexer_init(pipefd[0], &lex) == 0);

	test_tokens(&lex, expected);
	close(pipefd[0]);

	testi++;
}

/*
 * Tests
 */

START_TEST (test_symbols) {
	puts("--- test_symbols ---");
	testi = 0;

	test("+-/,{}()<>&*", (thToken[]) {
		th_token_create(thPlusTk, NULL, 0, 1),
		th_token_create(thDashTk, NULL, 1, 1),
		th_token_create(thSlashTk, NULL, 2, 1),
		th_token_create(thCommaTk, NULL, 3, 1),
		th_token_create(thOpenBraceTk, NULL, 4, 1),
		th_token_create(thCloseBraceTk, NULL, 5, 1),
		th_token_create(thOpenParenTk, NULL, 6, 1),
		th_token_create(thCloseParenTk, NULL, 7, 1),
		th_token_create(thLessThanTk, NULL, 8, 1),
		th_token_create(thGreaterThanTk, NULL, 9, 1),
		th_token_create(thAmpersandTk, NULL, 10, 1),
		th_token_create(thAsteriskTk, NULL, 11, 1),
		th_token_create(thEOITk, NULL, 12, 1)
	});

	test("===!==", (thToken[]) {
		th_token_create(thEqualsToTk, NULL, 0, 2),
		th_token_create(thEqualsTk, NULL, 2, 1),
		th_token_create(thDiffTk, NULL, 3, 2),
		th_token_create(thEqualsTk, NULL, 5, 1),
		th_token_create(thEOITk, NULL, 6, 1)
	});
}
END_TEST

START_TEST (test_numbers) {
	puts("--- test_numbers ---");
	testi = 0;

	char *literal = "0123456789";
	test(literal, (thToken[]) {
		th_token_create(thIntTk, literal, 0, 10),
		th_token_create(thEOITk, NULL, 10, 1)
	});

	literal = "123456789012345678890";
	test(literal, (thToken[]) {
		th_token_create(thIntTk, literal, 0, 21),
		th_token_create(thEOITk, NULL, 21, 1)
	});

	literal = "0";
	test(literal, (thToken[]) {
		th_token_create(thIntTk, literal, 0, 1),
		th_token_create(thEOITk, NULL, 1, 1)
	});

	literal = "1";
	test(literal, (thToken[]) {
		th_token_create(thIntTk, literal, 0, 1),
		th_token_create(thEOITk, NULL, 1, 1)
	});

	literal = "23";
	test(literal, (thToken[]) {
		th_token_create(thIntTk, literal, 0, 2),
		th_token_create(thEOITk, NULL, 2, 1)
	});

	literal = "8";
	test(literal, (thToken[]) {
		th_token_create(thIntTk, literal, 0, 1),
		th_token_create(thEOITk, NULL, 1, 1)
	});

	literal = ".123";
	test(literal, (thToken[]) {
		th_token_create(thUnknownTk, literal, 0, 1),
		th_token_create(thIntTk, literal, 1, 3),
		th_token_create(thEOITk, NULL, 4, 1)
	});

	literal = "123.456";
	test(literal, (thToken[]) {
		th_token_create(thFloatTk, literal, 0, 7),
		th_token_create(thEOITk, NULL, 7, 1)
	});

	literal = ".1.2";
	test(literal, (thToken[]) {
		th_token_create(thUnknownTk, literal, 0, 1),
		th_token_create(thFloatTk, literal, 1, 3),
		th_token_create(thEOITk, NULL, 4, 1)
	});
}
END_TEST

START_TEST (test_identifiers) {
	puts("--- test_identifiers ---");
	testi = 0;

	char *literal = "abc";
	test(literal, (thToken[]) {
		th_token_create(thIdentifierTk, literal, 0, 3),
		th_token_create(thEOITk, NULL, 3, 1)
	});

	literal = "a12";
	test(literal, (thToken[]) {
		th_token_create(thIdentifierTk, literal, 0, 3),
		th_token_create(thEOITk, NULL, 3, 1)
	});

	literal = "a__";
	test(literal, (thToken[]) {
		th_token_create(thIdentifierTk, literal, 0, 3),
		th_token_create(thEOITk, NULL, 3, 1)
	});

	literal = "a_34";
	test(literal, (thToken[]) {
		th_token_create(thIdentifierTk, literal, 0, 4),
		th_token_create(thEOITk, NULL, 4, 1)
	});

	literal = "a34_";
	test(literal, (thToken[]) {
		th_token_create(thIdentifierTk, literal, 0, 4),
		th_token_create(thEOITk, NULL, 4, 1)
	});

	literal = "_a3";
	test(literal, (thToken[]) {
		th_token_create(thIdentifierTk, literal, 0, 3),
		th_token_create(thEOITk, NULL, 3, 1)
	});

	literal = "_4v";
	test(literal, (thToken[]) {
		th_token_create(thIdentifierTk, literal, 0, 3),
		th_token_create(thEOITk, NULL, 3, 1)
	});
}
END_TEST

START_TEST (test_keywords) {
	puts("--- test_keywords ---");
	testi = 0;

	test("import",
		(thToken[]) {
			th_token_create(thImportKw, NULL, 0, 6),
			th_token_create(thEOITk, NULL, 6, 1)
		});

	char *literal = "aimport";
	test(literal, (thToken[]) {
		th_token_create(thIdentifierTk, literal, 0, 7),
		th_token_create(thEOITk, NULL, 7, 1)
	});

	literal = "importa";
	test(literal, (thToken[]) {
		th_token_create(thIdentifierTk, literal, 0, 7),
		th_token_create(thEOITk, NULL, 7, 1)
	});

	literal = "impor";
	test(literal, (thToken[]) {
		th_token_create(thIdentifierTk, literal, 0, 5),
		th_token_create(thEOITk, NULL, 5, 1)
	});

	test("pub", (thToken[]) {
		th_token_create(thPubKw, NULL, 0, 3),
		th_token_create(thEOITk, NULL, 3, 1)
	});

	test("local", (thToken[]) {
		th_token_create(thLocalKw, NULL, 0, 5),
		th_token_create(thEOITk, NULL, 5, 1)
	});

	test("type", (thToken[]) {
		th_token_create(thTypeKw, NULL, 0, 4),
		th_token_create(thEOITk, NULL, 4, 1)
	});

	test("as", (thToken[]) {
		th_token_create(thAsKw, NULL, 0, 2),
		th_token_create(thEOITk, NULL, 2, 1)
	});

	test("not", (thToken[]) {
		th_token_create(thNotKw, NULL, 0, 3),
		th_token_create(thEOITk, NULL, 3, 1)
	});

	test("and", (thToken[]) {
		th_token_create(thAndKw, NULL, 0, 3),
		th_token_create(thEOITk, NULL, 3, 1)
	});

	test("or", (thToken[]) {
		th_token_create(thOrKw, NULL, 0, 2),
		th_token_create(thEOITk, NULL, 2, 1)
	});

	test("fn", (thToken[]) {
		th_token_create(thFnKw, NULL, 0, 2),
		th_token_create(thEOITk, NULL, 2, 1)
	});

	test("return", (thToken[]) {
		th_token_create(thReturnKw, NULL, 0, 6),
		th_token_create(thEOITk, NULL, 6, 1)
	});
}
END_TEST

START_TEST (test_comments) {
	puts("--- test_comments ---");
	testi = 0;  
  
	test("-- abc", (thToken[]) {
		th_token_create(thEOITk, NULL, 6, 1)
	});

	test("- -- this is a dash token :)", (thToken[]) {
		th_token_create(thDashTk, NULL, 0, 1),
		th_token_create(thEOITk, NULL, 28, 1)
	});

	char *literal = "abc == 2 {\n-- Do something here idk\n}";
	test(literal, (thToken[]) {
		th_token_create(thIdentifierTk, literal, 0, 3),
		th_token_create(thEqualsToTk, NULL, 4, 2),
		th_token_create(thIntTk, literal, 7, 1),
		th_token_create(thOpenBraceTk, NULL, 9, 1),
		th_token_create(thCloseBraceTk, NULL, 36, 1),
		th_token_create(thEOITk, NULL, 37, 1)
	});

	test("-- +-/,{}()<>===!=&*", (thToken[]) {
		th_token_create(thEOITk, NULL, 20, 1)
	});
}
END_TEST

START_TEST (test_whitespaces) {
	puts("--- test_whitespaces ---");
	testi = 0;

	test("\t-\t", (thToken[]) {
		th_token_create(thDashTk, NULL, 1, 1),
		th_token_create(thEOITk, NULL, 3, 1)
	});

	char *literal = "\nab\n";
	test(literal, (thToken[]) {
		th_token_create(thIdentifierTk, literal, 1, 2),
		th_token_create(thEOITk, NULL, 4, 1)
	});

	literal = " 123 ";
	test(literal, (thToken[]) {
		th_token_create(thIntTk, literal, 1, 3),
		th_token_create(thEOITk, NULL, 5, 1)
	});
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
