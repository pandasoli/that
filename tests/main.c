#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <thatlang/lexer/lexer.h>
#include <thatlang/parser/parser.h>
#include <thatlang/diags/bag.h>
#include <intern/debug.h>


void print_node(thNode *node, char *last_indent) {
	char indent[strlen(last_indent) + 2];
	memset(indent, ' ', sizeof(indent));
	indent[sizeof(indent)] = 0;

	switch (node->kind) {
		case NumberNk:
			printf("\e[2m");
			print_token(node->number);
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
	}

	if (node->next) {
		printf("%s", last_indent);
		print_node(node->next, last_indent);
	}
}

int main(void) {
	char literal[] = "12 + 4 / (1 + 2)";

	int fd[2];
	pipe(fd);

	write(fd[1], literal, strlen(literal));
	close(fd[1]);

	thLexer lex;
	th_lexer_init(fd[0], &lex);

	thParser par;
	th_parser_init((thERR (*)(void *, thToken *)) lex.lex, &lex, &par);

	thNode *node;
	thERR err = par.parse(&par, &node);

	close(fd[0]);

	if (err) {
		printf("err: %d\n", err);

		thDiag *current = th_diags.intern;

		while (current != NULL) {
			printf("%s\n", current->msg);
			current = current->next;
		}

		current = th_diags.diags;

		while (current != NULL) {
			printf("%s\n", current->msg);
			current = current->next;
		}


		return 1;
	}

	print_node(node, "");

	return 0;
}
