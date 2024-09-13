#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <thatlang/lexer/lexer.h>
#include <thatlang/parser/parser.h>
#include <thatlang/parser/node.h>
#include <thatlang/diags/bag.h>
#include <intern/debug.h>


char *op_str(thTokenKind kind) {
	return (
		kind == thPlusTk        ? "+" :
		kind == thDashTk        ? "-" :
		kind == thSlashTk       ? "/" :
		kind == thEqualsToTk    ? "==" :
		kind == thDiffTk        ? "!=" :
		kind == thLessThanTk    ? "<" :
		kind == thGreaterThanTk ? ">" :
		kind == thEqualsTk      ? "=" :
		"?"
	);
}

void print_node(thNode *node, int lindent);
void print_type(thTypeNode *node) {
	while (node) {
		switch (node->kind) {
			case thIdentifierNk: printf("%*s", (int) node->identifier.location.len, node->identifier.literal); break;
			case thFnNk:
				printf("\e[90mfn\e[m");

				if (node->fn.args) {
					putchar('(');

					thArgs *current = node->fn.args;
					while (current != NULL) {
						printf("\e[30m");
						print_type(current->type);
						printf("\e[m");
						current = current->next;
						if (current) printf(", ");
					}

					putchar(')');
				}

				if (node->fn.ret_type) printf(" \e[30m");
				print_type(node->fn.ret_type);
				break;

			default: break;
		}

		node = node->next;
		if (node) printf(" | ");
	}
}

void print_node(thNode *node, int lindent) {
	int indent = lindent + 2;

	switch (node->kind) {
		case thNumberNk:     printf("\e[2m%.*s\e[m", (int) node->number.location.len,     node->number.literal);     break;
		case thIdentifierNk: printf("\e[3m%.*s\e[m", (int) node->identifier.location.len, node->identifier.literal); break;
		case thReferenceNk: printf("&\e[3m%.*s\e[m", (int) node->reference.location.len, node->reference.literal); break;
		case thDereferenceNk: printf("*\e[3m%.*s\e[m", (int) node->dereference.location.len, node->dereference.literal); break;

		case thFnNk:
			printf("\e[90mfn\e[m");

			if (node->fn.args) {
				putchar('(');

				thArgs *current = node->fn.args;
				while (current != NULL) {
					printf("\e[3m%.*s\e[m ", (int) current->identifier.location.len, current->identifier.literal);

					printf("\e[30m");
					print_type(current->type);
					printf("\e[m");
					current = current->next;
					if (current) printf(", ");
				}

				putchar(')');
			}

			if (node->fn.ret_type) putchar(' ');
			printf("\e[30m");
			print_type(node->fn.ret_type);

			if (node->fn.body) {
				printf("\e[m {\n%*s", indent, "");
				print_node(node->fn.body, indent);
				printf("\n%*s}\n", lindent, "");
			}
			break;

		case thUnaryNk:
			puts("Unary [");

			printf("%*s", indent, "");
			printf("%s\n\e[0m", op_str(node->unary.op));

			print_node(node->unary.val, indent);

			printf("%*s]\n", lindent, "");
			break;

		case thBinaryNk:
			print_node(node->binary.left, indent);
			printf(" \e[34m%s\e[m ", op_str(node->binary.op));
			print_node(node->binary.right, indent);

			if (node->binary.op == thEqualsTk)
				putchar('\n');
			break;

		case thAssignNk:
			printf("\e[90mlocal\e[m ");
			printf("\e[3m%.*s\e[m ", (int) node->assign.name.location.len, node->assign.name.literal);

			printf("\e[30m");
			print_type(node->assign.type);
			printf("\e[m");
	}

	if (node->next) {
		printf("%*s", lindent, "");
		print_node(node->next, lindent);
	}
}

void print_diags(thERR err) {
	printf("err: %d\n", err);

	thDiag *current;

	current = th_diags.intern;
	while (current != NULL) {
		printf("intern (%zu) %s\n", current->location.pos, current->msg);
		current = current->next;
	}

	current = th_diags.diags;
	while (current != NULL) {
		printf("diags (%zu) %s\n", current->location.pos, current->msg);
		current = current->next;
	}
}

int main(void) {
	char literal[] =
		"local a u8 = 12 + 4"
		"local b ptr = &a "
		""
		"local b fn(u8, u8) u8 = fn(a u8, b u8) u8 { a < c }";
		/*"local b fn(u8) u8 = fn(c u8) u8 {"*/
		/*	"a < c and a or c"*/
		/*"}";*/

	int fd[2];
	pipe(fd);

	write(fd[1], literal, strlen(literal));
	close(fd[1]);

	thERR err;

	thLexer lex;
	thParser par;
	thNode *node = {};

	err = th_lexer_init(fd[0], &lex);
	if (!err) {
		err = th_parser_init((thERR (*)(void *, thToken *)) lex.lex, &lex, &par);
		if (!err) {
			err = par.parse(&par, &node);
		}
	}

	close(fd[0]);

	print_diags(err);
	if (node) print_node(node, 0);

	return 0;
}
