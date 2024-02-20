#include <stdio.h>
#include <stdlib.h>
#include "parser/node.h"
#include "diags/bag.h"


ERR heap_node(Node *node, DiagBag *bag, Node **out) {
	Node *res = malloc(sizeof(Node));

	if (res == NULL) {
		bag->report_intern(bag, "malloc(%zu) returned NULL on heap_node.", sizeof(Node));
		return 2;
	}

	*res = *node;
	*out = res;

	return 0;
}

void free_node(Node *node) {
	switch (node->kind) {
		case UnaryNk:
			free_node(node->unary.val);
			break;

		case BinaryNk:
			free_node(node->binary.left);
			free_node(node->binary.right);
			break;

		default:
			free(node);
	}
}
