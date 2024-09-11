#include <thatlang/parser/node.h>
#include <intern/parser/node.h>
#include <intern/diag_bag.h>

#include <string.h>


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

thERR node_fn_create(thArgs *args, thTypeNode *ret_type, thNode *body, thNode **out) {
	thNode node = { thFnNk, .fn = (thFnNode) { args, ret_type, body } };

	thNode *res = malloc(sizeof *res);
	if (res == NULL) {
		report_intern("malloc(%zu) returned NULL on %s", sizeof *res, __func__);
		return 1;
	}

	memcpy(res, &node, sizeof *res);
	*out = res;

	return 0;
}

thERR node_args_create(thToken identifier, thTypeNode *type, thArgs **out) {
	thArgs args = { identifier, type, NULL };

	thArgs *res = malloc(sizeof *res);
	if (res == NULL) {
		report_intern("malloc(%zu) returned NULL on %s", sizeof *res, __func__);
		return 1;
	}

	memcpy(res, &args, sizeof *res);
	*out = res;

	return 0;
}

thERR node_assign_create(thToken name, thTypeNode *type, thNode *val, thNode **out) {
	thNode node = { thAssignNk, .assign = (thAssignNode) { name, type, val } };

	thNode *res = malloc(sizeof *res);
	if (res == NULL) {
		report_intern("malloc(%zu) returned NULL on %s", sizeof *res, __func__);
		return 1;
	}

	memcpy(res, &node, sizeof *res);
	*out = res;

	return 0;
}

thERR node_binary_create(thNode *left, thTokenKind op, thNode *right, thNode **out) {
	thNode node = { thBinaryNk, .binary = (thBinaryNode) { left, op, right } };

	thNode *res = malloc(sizeof *res);
	if (res == NULL) {
		report_intern("malloc(%zu) returned NULL on %s", sizeof *res, __func__);
		return 1;
	}

	memcpy(res, &node, sizeof *res);
	*out = res;

	return 0;
}

thERR node_unary_create(thTokenKind op, thNode *val, thNode **out) {
	thNode node = { thUnaryNk, .unary = (thUnaryNode) { op, val } };

	thNode *res = malloc(sizeof *res);
	if (res == NULL) {
		report_intern("malloc(%zu) returned NULL on %s", sizeof *res, __func__);
		return 1;
	}

	memcpy(res, &node, sizeof *res);
	*out = res;

	return 0;
}

thERR node_identifier_create(thToken identifier, thNode **out) {
	thNode node = { thIdentifierNk, .identifier = identifier };

	thNode *res = malloc(sizeof *res);
	if (res == NULL) {
		report_intern("malloc(%zu) returned NULL on %s", sizeof *res, __func__);
		return 1;
	}

	memcpy(res, &node, sizeof *res);
	*out = res;

	return 0;
}


thERR node_number_create(thToken val, thNode **out) {
	thNode node = { thNumberNk, .number = val };

	thNode *res = malloc(sizeof *res);
	if (res == NULL) {
		report_intern("malloc(%zu) returned NULL on %s", sizeof *res, __func__);
		return 1;
	}

	memcpy(res, &node, sizeof *res);
	*out = res;

	return 0;
}
