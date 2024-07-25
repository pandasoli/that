#include <thatlang/lexer/token.h>
#include <thatlang/parser/parser.h>
#include <intern/parser/node.h>
#include <intern/diag_bag.h>
#include <intern/utils.h>

#define CURRENT self->current
#define NEXT                                            \
		if ((err = self->next(self->next_props, &CURRENT))) \
			return err


/* Precedence order
 * | - [x] Assignment
 * | - [ ] Member
 * v - [ ] Function
 *   - [ ] Logical
 *   - [ ] Comparison
 *   - [x] Additive
 *   - [x] Multiplicative
 *   - [x] Unary
 *   - [x] Primary
 *
 * I decided to keep addition and multiplication separated for better understanding
 * (not like https://github.com/pandasoli/minsk/blob/master/Minsk/CodeAnalysis/Syntax/Parser.cs#L86)
 */

static thERR assignment(thParser *self, thNode **node);

static thERR primary(thParser *self, thNode **node) {
	thERR err;

	switch (CURRENT.kind) {
		case FloatTk:
		case IntTk:
			if ((err = node_number_create(CURRENT, node)))
				return err;
			break;
		
		/*case IdentifierTk:*/
		/*	node->kind = VarCallNk;*/
		/*	node->var_call = CURRENT;*/
		/*	break;*/

		case OpenParenTk: {
			NEXT;
			// TODO: This call to `assignment` says: Unexpected CloseParen token found parsing primary
			// For this issue C gives the error: Expected expression

			if ((err = assignment(self, node)))
				return err;

			// Check for closing parenthesis
			if (CURRENT.kind != CloseParenTk) {
				report(CURRENT.location, "NextFn returned %s when expected %s parsing parenthesized expression", token_strkind(CURRENT.kind), token_strkind(CloseParenTk));
				return 1;
			}

		} break;

		default:
			report(CURRENT.location, "Unexpected %s token found parsing primary", token_strkind(CURRENT.kind));
			return 1;
	}

	NEXT;

	return 0;
}

static thERR unary(thParser *self, thNode **node) {
	thERR err;

	if (CURRENT.kind == DashTk)
	{
		// Get operation
		thToken op = CURRENT;

		NEXT;

		// Get value
		thNode *value;

		if ((err = primary(self, &value)))
			return err;

		// Create node
		if ((err = node_unary_create(op, value, node)))
			return err;
	}
	else
		return primary(self, node);

	return 0;
}

static thERR multiplicative(thParser *self, thNode **node) {
	thERR err;

	if ((err = unary(self, node)))
		return err;

	while (CURRENT.kind == SlashTk) {
		// Get operator
		thToken op = CURRENT;

		NEXT;

		// Get value
		thNode *right;

		if ((err = unary(self, &right)))
			return err;

		// Create node
		if ((err = node_binary_create(*node, op, right, node)))
			return err;
	}

	return 0;
}

static thERR additive(thParser *self, thNode **node) {
	thERR err;

	if ((err = multiplicative(self, node)))
		return err;

	while (CURRENT.kind == PlusTk || CURRENT.kind == DashTk) {
		// Get operator
		thToken op = CURRENT;

		NEXT;

		// Get value
		thNode *right;

		if ((err = multiplicative(self, &right)))
			return err;

		// Create node
		if ((err = node_binary_create(*node, op, right, node)))
			return err;
	}

	return 0;
}

static thERR assignment(thParser *self, thNode **node) {
	return additive(self, node);
	/*thERR err;*/
	/**/
	/*if (CURRENT.kind != LocalTk)*/
	/*	return additive(self, node);*/
	/**/
	/*// Get identifier*/
	/*if ((NEXT).kind != IdentifierTk) {*/
	/*	report(CURRENT.location, "NextFn returned %s when expected %s parsing variable definition", token_strkind(CURRENT.kind), token_strkind(IdentifierTk));*/
	/*	return 1;*/
	/*}*/
	/**/
	/*thToken id = CURRENT;*/
	/**/
	/*// Get type*/
	/*// TODO: implement type parsing*/
	/*if ((NEXT).kind != IdentifierTk) {*/
	/*	report(CURRENT.location, "NextFn returned %s when expected %s parsing variable definition", token_strkind(CURRENT.kind), token_strkind(IdentifierTk));*/
	/*	return 1;*/
	/*}*/
	/**/
	/*thToken type = CURRENT;*/
	/**/
	/*// Check for equals sign*/
	/*if ((NEXT).kind != EqualsTk) {*/
	/*	report(CURRENT.location, "NextFn returned %s when expected %s parsing variable definition", token_strkind(CURRENT.kind), token_strkind(EqualsTk));*/
	/*	return 1;*/
	/*}*/
	/**/
	/*NEXT;*/
	/**/
	/*// Get value*/
	/*if ((err = additive(self, node)))*/
	/*	return err;*/
	/**/
	/*// Create node*/
	/*if ((err = th_heap_node(node, &self->diags, &node->assignment.val)))*/
	/*	return err;*/
	/**/
	/*node->kind = AssignmentNk;*/
	/*node->assignment.identifier = id;*/
	/**/
	/*return 0;*/
}

static thERR parse(thParser *self, thNode **node) {
	thERR err;
	NEXT;

	thNode *last_node = NULL;
	*node = NULL;

	while (CURRENT.kind != EOFTk) {
		thNode *new;

		if ((err = assignment(self, &new)))
			return err;

		// Linked list append
		last_node = *node == NULL
			? (*node = new)
			: (last_node->next = new);
	}

	return 0;
}

thERR th_parser_init(thNextFn next, void *next_props, thParser *parser) {
	if (next <= 0 || parser == NULL)
		return 1;

	parser->current = (thToken) {};
	parser->next = next;
	parser->next_props = next_props;

	parser->parse = &parse;

	return 0;
}
