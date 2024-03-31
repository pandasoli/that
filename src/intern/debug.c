#include <stdio.h>
#include <string.h>
#include "parser/node.h"
#include "lexer/token.h"
#include "debug.h"
#include "utils.h"


void print_token(Token token) {
	printf(
		"%s \"%.*s\" %zu %zu",
		token_strkind(token.kind),
		(int) token.span.len, token.literal,
		token.span.pos,
		token.span.len
	);
}

void print_node(Node *node, char *last_indent) {
	char indent[strlen(last_indent) + 2];
	memset(indent, ' ', sizeof(indent));
	indent[sizeof(indent)] = 0;

	switch (node->kind) {
		case NumberNk:
			printf("\e[2m");
			print_token(node->number);
			printf("\n\e[0m");
			break;

		case VarCallNk:
			printf("\e[3m");
			print_token(node->var_call);
			printf("\n\e[0m");
			break;

		case UnaryNk:
			puts("Unary [");

			printf("%s", indent);
			print_token(node->unary.op);
			printf("\n\e[0m");

			print_node(node->unary.val, indent);

			printf("%s]\n", last_indent);
			break;

		case BinaryNk:
			puts("Binary [");

			printf("%s", indent);
			print_node(node->binary.left, indent);

			printf("%s\e[3m", indent);
			print_token(node->binary.op);
			printf("\n\e[0m");

			printf("%s", indent);
			print_node(node->binary.right, indent);

			printf("%s]\n", last_indent);
			break;

		case VarDeclarationNk:
			puts("VarDeclaration [");

			printf("%s\e[3m", indent);
			print_token(node->var_declaration.identifier);
			printf("\n\e[0m");

			printf("%s\e[30m", indent);
			print_token(node->var_declaration.type);
			printf("\n\e[0m");

			if (node->var_declaration.val) {
				printf("%s", indent);
				print_node(node->var_declaration.val, indent);
			}

			printf("%s]\n", last_indent);
			break;

		case AssignmentNk:
			puts("Assignment [");

			printf("%s\e[3m", indent);
			print_token(node->assignment.identifier);
			printf("\n\e[0m");

			printf("%s", indent);
			print_node(node->assignment.val, indent);

			printf("%s]\n", last_indent);
	}

	if (node->next) {
		printf("%s", last_indent);
		print_node(node->next, last_indent);
	}
}
