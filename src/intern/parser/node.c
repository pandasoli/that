#include <thatlang/parser/node.h>
#include <intern/parser/node.h>
#include <intern/diag_bag.h>


thERR node_type_create(thTypeNode *type, thNode **out) {
	thNode *res = malloc(sizeof *res);
	if (res == NULL) {
		report_intern("malloc(%zu) returned NULL on %s", sizeof *res, __func__);
		return 1;
	}

	switch ((res->kind = type->kind)) {
		case thNumberNk:     res->number     = type->number;     break;
		case thIdentifierNk: res->identifier = type->identifier; break;
		case thFnNk:         res->fn         = type->fn;         break;
		case thBinaryNk:     res->binary     = type->binary;     break;
		case thUnaryNk:      res->unary      = type->unary;      break;
		case thAssignNk:     res->assign     = type->assign;     break;
	}

	*out = res;

	return 0;

}

thERR node_fn_create(thTypeNode *ret_type, thNode *body, thNode **out) {
	thNode *res = malloc(sizeof *res);
	if (res == NULL) {
		report_intern("malloc(%zu) returned NULL on %s", sizeof *res, __func__);
		return 1;
	}

	res->kind = thFnNk;
	res->fn.ret_type = ret_type;
	res->fn.body = body;

	*out = res;

	return 0;

}

thERR node_identifier_create(thToken identifier, thNode **out) {
	thNode *res = malloc(sizeof *res);
	if (res == NULL) {
		report_intern("malloc(%zu) returned NULL on %s", sizeof *res, __func__);
		return 1;
	}

	res->kind = thIdentifierNk;
	res->identifier = identifier;

	*out = res;

	return 0;
}

thERR node_assign_create(thToken name, thTypeNode *type, thNode *val, thNode **out) {
	thNode *res = malloc(sizeof *res);
	if (res == NULL) {
		report_intern("malloc(%zu) returned NULL on %s", sizeof *res, __func__);
		return 1;
	}

	res->kind = thAssignNk;
	res->assign.name = name;
	res->assign.type = type;
	res->assign.val = val;

	*out = res;

	return 0;
}

thERR node_binary_create(thNode *left, thToken op, thNode *right, thNode **out) {
	thNode *res = malloc(sizeof *res);
	if (res == NULL) {
		report_intern("malloc(%zu) returned NULL on %s", sizeof *res, __func__);
		return 1;
	}

	res->kind = thBinaryNk;
	res->binary.left = left;
	res->binary.op = op;
	res->binary.right = right;

	*out = res;

	return 0;
}

thERR node_unary_create(thToken op, thNode *val, thNode **out) {
	thNode *res = malloc(sizeof *res);
	if (res == NULL) {
		report_intern("malloc(%zu) returned NULL on %s", sizeof *res, __func__);
		return 1;
	}

	res->kind = thUnaryNk;
	res->unary.op = op;
	res->unary.val = val;

	*out = res;

	return 0;
}

thERR node_number_create(thToken val, thNode **out) {
	thNode *res = malloc(sizeof *res);
	if (res == NULL) {
		report_intern("malloc(%zu) returned NULL on %s", sizeof *res, __func__);
		return 1;
	}

	res->kind = thNumberNk;
	res->number = val;

	*out = res;

	return 0;
}
