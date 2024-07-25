#include <thatlang/parser/node.h>
#include <intern/parser/node.h>
#include <intern/diag_bag.h>


thERR node_binary_create(thNode *left, thToken op, thNode *right, thNode **out) {
	thNode *res = malloc(sizeof *res);
	if (res == NULL) {
		report_intern("malloc(%zu) returned NULL on node_binary_create", sizeof *res);
		return 2;
	}

	res->kind = BinaryNk;
	res->binary.left = left;
	res->binary.op = op;
	res->binary.right = right;

	*out = res;

	return 0;
}

thERR node_unary_create(thToken op, thNode *val, thNode **out) {
	thNode *res = malloc(sizeof *res);
	if (res == NULL) {
		report_intern("malloc(%zu) returned NULL on node_unary_create", sizeof *res);
		return 2;
	}

	res->kind = UnaryNk;
	res->unary.op = op;
	res->unary.val = val;

	*out = res;

	return 0;
}

thERR node_number_create(thToken val, thNode **out) {
	thNode *res = malloc(sizeof *res);
	if (res == NULL) {
		report_intern("malloc(%zu) returned NULL on node_number_create", sizeof *res);
		return 2;
	}

	res->kind = NumberNk;
	res->number = val;

	*out = res;

	return 0;
}
