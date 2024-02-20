#include <stdio.h>
#include <stdlib.h>
#include <check.h>
#include "lexer/lexer.h"
#include "lexer/token.h"
#include "parser/node.h"
#include "parser/parser.h"
#include "debug.h"
#include "utils.h"


/*
 * Helpers
 */

typedef struct {
	char *literal;
	Node *expected;
} Test;

static Node *new_numnode(Token number, Node *next) {
	Node *node = malloc(sizeof(Node));
	node->kind = NumberNk;
	node->number = number;
	node->next = next;
	return node;
}

static Node *new_varcallnode(Token identifier, Node *next) {
	Node *node = malloc(sizeof(Node));
	node->kind = VarCallNk;
	node->var_call = identifier;
	node->next = next;
	return node;
}

static Node *new_binnode(Node *left, Token op, Node *right) {
	Node *node = malloc(sizeof(Node));
	node->kind = BinaryNk;
	node->binary.left = left;
	node->binary.op = op;
	node->binary.right = right;
	return node;
}

static Node *new_unanode(Token op, Node *val) {
	Node *node = malloc(sizeof(Node));
	node->kind = UnaryNk;
	node->unary.op = op;
	node->unary.val = val;
	return node;
}

static Node *new_assignnode(Token identifier, Node *val, Node *next) {
	Node *node = malloc(sizeof(Node));
	node->kind = AssignmentNk;
	node->assignment.identifier = identifier;
	node->assignment.val = val;
	node->next = next;
	return node;
}

static Node *new_vardecnode(Token identifier, Token type, Node *val, Node *next) {
	Node *node = malloc(sizeof(Node));
	node->kind = VarDeclarationNk;
	node->var_declaration.identifier = identifier;
	node->var_declaration.type = type;
	node->var_declaration.val = val;
	node->next = next;
	return node;
}

static void parse(char *literal, Node **ast) {
	Lexer lex;
	ck_assert(lexer_init(literal, &lex) == 0);

	Parser par;
	ck_assert(parser_init((NextFn) lex.lex, &lex, &par) == 0);

	ERR err = par.parse(&par, ast);

	{
		Diag *current = par.diags.diags;
		while (current != NULL) {
			puts(current->msg);
			current = current->next;
		}
	}

	ck_assert(err == 0);
	par.diags.free(&par.diags);
}

/*
 * Testing
 */

static void test_token(Token actual, Token expected) {
	print_token(actual);
	putchar('\n');

	ck_assert_msg(actual.kind == expected.kind, "Assert tokens' kind (%s and %s) failed", token_strkind(actual.kind), token_strkind(expected.kind));
	ck_assert_msg(actual.span.pos == expected.span.pos, "Assert tokens' pos (%zu and %zu)", actual.span.pos, expected.span.pos);
	ck_assert_msg(actual.span.len == expected.span.len, "Assert tokens' len (%zu and %zu)", actual.span.len, expected.span.len);
	ck_assert_msg(actual.literal == expected.literal, "Assert tokens' literal (%p and %p)", actual.literal, expected.literal);
}

static void test_ast(Node *actual, Node *expected, char *last_indent) {
	ck_assert_msg(actual->kind == expected->kind, "Assert nodes' kind (%s and %s) failed", node_strkind(actual->kind), node_strkind(expected->kind));

	char indent[strlen(last_indent) + 2];
	memset(indent, ' ', sizeof(indent));
	indent[sizeof(indent)] = 0;

	switch (actual->kind) {
		case NumberNk:
			test_token(actual->number, expected->number);
			break;

		case VarCallNk:
			test_token(actual->var_call, expected->var_call);
			break;

		case BinaryNk:
			puts("Binary: [");

			printf("%s", indent);
			test_ast(actual->binary.left, expected->binary.left, indent);

			printf("%s", indent);
			test_token(actual->binary.op, expected->binary.op);

			printf("%s", indent);
			test_ast(actual->binary.right, expected->binary.right, indent);

			printf("%s]\n", last_indent);
			break;

		case UnaryNk:
			puts("Unary: [");

			printf("%s", indent);
			test_token(actual->unary.op, expected->unary.op);

			printf("%s", indent);
			test_ast(actual->unary.val, expected->unary.val, indent);

			printf("%s]\n", last_indent);
			break;

		case VarDeclarationNk:
			puts("VarDeclaration [");

			printf("%s", indent);
			test_token(actual->var_declaration.identifier, expected->var_declaration.identifier);

			printf("%s", indent);
			test_token(actual->var_declaration.type, expected->var_declaration.type);

			ck_assert(
				(actual->var_declaration.val && expected->var_declaration.val) ||
				(!actual->var_declaration.val && !expected->var_declaration.val)
			);

			if (actual->var_declaration.val) {
				printf("%s", indent);
				test_ast(actual->var_declaration.val, expected->var_declaration.val, indent);
			}

			printf("%s]\n", last_indent);
			break;

		case AssignmentNk:
			puts("Assignment [");

			printf("%s", indent);
			test_token(actual->assignment.identifier, expected->assignment.identifier);

			printf("%s", indent);
			test_ast(actual->assignment.val, expected->assignment.val, indent);

			printf("%s]\n", last_indent);
	}

	ck_assert(!actual->next == !expected->next);
	if (actual->next) test_ast(actual->next, expected->next, last_indent);
}

static void test_many(Test *tests) {
	for (int i = 0; tests[i].literal != 0; i++) {
		char *literal = tests[i].literal;
		Node *expected = tests[i].expected;

		printf("- \e[2m\"%s\"\e[0m\n", literal);

		Node *ast;
		parse(literal, &ast);
		test_ast(ast, expected, "");

		free_node(ast);
		free_node(expected);
	}
}

/*
 * Tests
 */

START_TEST (test_number_nodes) {
	char *literal_1 = "-3";
	Node *expected_1 = new_unanode(
		new_token(DashTk, literal_1, 0, 1),
		new_numnode(new_token(IntTk, literal_1, 1, 1), NULL)
	);

	char *literal_2 = "-456";
	Node *expected_2 = new_unanode(
		new_token(DashTk, literal_2, 0, 1),
		new_numnode(new_token(IntTk, literal_2, 1, 3), NULL)
	);

	char *literal_3 = "+35";
	Node *expected_3 = new_unanode(
		new_token(PlusTk, literal_3, 0, 1),
		new_numnode(new_token(IntTk, literal_3, 1, 2), NULL)
	);

	char *literal_4 = "-4/5";
	Node *expected_4 = new_binnode(
		new_unanode(
			new_token(DashTk, literal_4, 0, 1),
			new_numnode(new_token(IntTk, literal_4, 1, 1), NULL)
		),
		new_token(SlashTk, literal_4, 2, 1),
		new_numnode(new_token(IntTk, literal_4, 3, 1), NULL)
	);

	char *literal_5 = "31423*5";
	Node *expected_5 = new_binnode(
		new_numnode(new_token(IntTk, literal_5, 0, 5), NULL),
		new_token(AsteriskTk, literal_5, 5, 1),
		new_numnode(new_token(IntTk, literal_5, 6, 1), NULL)
	);

	Test tests[] = {
		// Unary
		{literal_1, expected_1},
		{literal_2, expected_2},
		{literal_3, expected_3},

		// Binary
		{literal_4, expected_4},
		{literal_5, expected_5},
		{0}
	};

	test_many(tests);
}
END_TEST

START_TEST (test_precedences) {
	char *literal_1 = "12- (4 -14 / 2) * (-2)";
	Node *expected_1 = new_binnode(
		new_numnode(new_token(IntTk, literal_1, 0, 2), NULL),
		new_token(DashTk, literal_1, 2, 1),
	  new_binnode(
  	  new_binnode(
				new_numnode(new_token(IntTk, literal_1, 5, 1), NULL),
				new_token(DashTk, literal_1, 7, 1),
	      new_binnode(
					new_numnode(new_token(IntTk, literal_1, 8, 2), NULL),
					new_token(SlashTk, literal_1, 11, 1),
					new_numnode(new_token(IntTk, literal_1, 13, 1), NULL)
				)
			),
			new_token(AsteriskTk, literal_1, 16, 1),
	    new_unanode(
				new_token(DashTk, literal_1, 19, 1),
				new_numnode(new_token(IntTk, literal_1, 20, 1), NULL)
			)
		)
	);

	Test tests[] = {
		{literal_1, expected_1},
		{0}
	};

	test_many(tests);
}
END_TEST

START_TEST (test_var_declaration) {
	char literal_1[] = "a int = 23";
	Node *expected_1 = new_vardecnode(
		new_token(IdentifierTk, literal_1, 0, 1),
		new_token(IdentifierTk, literal_1, 2, 3),
		new_numnode(new_token(IntTk, literal_1, 8, 2), NULL),
		NULL
	);

	char literal_2[] = "a int = 23 a";
	Node *expected_2 = new_vardecnode(
		new_token(IdentifierTk, literal_2, 0, 1),
		new_token(IdentifierTk, literal_2, 2, 3),
		new_numnode(new_token(IntTk, literal_2, 8, 2), NULL),
		new_varcallnode(new_token(IdentifierTk, literal_2, 11, 1), NULL)
	);

	char literal_3[] = "b a int = 23.4";
	Node *expected_3 = new_vardecnode(
		new_token(IdentifierTk, literal_3, 0, 1),
		new_token(IdentifierTk, literal_3, 2, 1),
		NULL,
		new_assignnode(
			new_token(IdentifierTk, literal_3, 4, 3),
			new_numnode(new_token(FloatTk, literal_3, 10, 4), NULL),
			NULL
		)
	);

	Test tests[] = {
		{literal_1, expected_1},
		{literal_2, expected_2},
		{literal_3, expected_3},
		{0}
	};

	test_many(tests);
}
END_TEST

START_TEST (test_assignment) {
	char literal_1[] = "sa = 34";
	Node *expected_1 = new_assignnode(
		new_token(IdentifierTk, literal_1, 0, 2),
		new_numnode(new_token(IntTk, literal_1, 5, 2), NULL),
		NULL
	);

	char literal_2[] = "1 sa = 34";
	Node *expected_2 = new_numnode(
		new_token(IntTk, literal_2, 0, 1),
		new_assignnode(
			new_token(IdentifierTk, literal_2, 2, 2),
			new_numnode(new_token(IntTk, literal_2, 7, 2), NULL),
			NULL
		)
	);

	char literal_3[] = "4 az = 12 4";
	Node *expected_3 = new_numnode(
		new_token(IntTk, literal_3, 0, 1),
		new_assignnode(
			new_token(IdentifierTk, literal_3, 2, 2),
			new_numnode(new_token(IntTk, literal_3, 7, 2), NULL),
			new_numnode(new_token(IntTk, literal_3, 10, 1), NULL)
		)
	);

	Test tests[] = {
		{literal_1, expected_1},
		{literal_2, expected_2},
		{literal_3, expected_3},
		{0}
	};

	test_many(tests);
}
END_TEST

/*
 * Main
 */

Suite *lexer_suite() {
	TCase *tc_core = tcase_create("Parser core");

	tcase_add_test(tc_core, test_number_nodes);
	tcase_add_test(tc_core, test_precedences);
	tcase_add_test(tc_core, test_assignment);
	tcase_add_test(tc_core, test_var_declaration);

	Suite *s = suite_create("Parser");
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
