#include <stdio.h>
#include "lexer/lexer.h"
#include "parser/node.h"
#include "parser/parser.h"
#include "debug.h"

int main() {
	char *literal = "b a int = 32 a";

	Lexer lex;
	lexer_init(literal, &lex);

	Parser par;
	parser_init((NextFn) lex.lex, &lex, &par);

	Node *ast;
	par.parse(&par, &ast);

	{
		Diag *current = par.diags.intern;
		while (current != NULL) {
			printf("%s\n", current->msg);
			current = current->next;
		}

		current = par.diags.diags;
		while (current != NULL) {
			printf("At %zu, %s\n", current->span.pos, current->msg);
			current = current->next;
		}
	}

	print_node(ast, "");

	free_node(ast);
	par.diags.free(&par.diags);

	return 0;
}
