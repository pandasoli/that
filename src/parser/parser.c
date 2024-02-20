#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser/parser.h"
#include "diags/bag.h"
#include "globl.h"
#include "lexer/token.h"
#include "parser/node.h"
#include "utils.h"
#include "debug.h"

#define CURRENT self->current
#define REPORT(span, format, ...) self->diags.report(&self->diags, span, format, __VA_ARGS__)
#define NEXT self->current = self->next(self->next_data)


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

static ERR assignment(Parser *self, Node *node);

static ERR primary(Parser *self, Node *node) {
	ERR err;

	switch (CURRENT.kind) {
		case FloatTk:
		case IntTk:
			node->kind = NumberNk;
			node->number = CURRENT;
			break;

		case IdentifierTk:
			node->kind = VarCallNk;
			node->var_call = CURRENT;
			break;

		case OpenParenTk:
			NEXT;

			if ((err = assignment(self, node)))
					return err;

			// Check for closing parenthesis
			if (CURRENT.kind != CloseParenTk) {
				REPORT(CURRENT.span, "NextFn returned %s when expected %s parsing parenthesized expression", token_strkind(CURRENT.kind), token_strkind(CloseParenTk), err);
				return 1;
			}

			break;

		default:
			REPORT(CURRENT.span, "Unexpected %s token found parsing primary", token_strkind(CURRENT.kind));
			return 1;
	}

	NEXT;

	return 0;
}

static ERR unary(Parser *self, Node *node) {
	ERR err;

	if (CURRENT.kind == DashTk)
	{
		// Get operation
		Token op = CURRENT;

		NEXT;

		// Get value
		Node value = {};

		if ((err = primary(self, &value)))
			return err;

		// Create node
		if ((err = heap_node(&value, &self->diags, &node->unary.val)))
			return err;

		node->unary.op = op;
		node->kind = UnaryNk;
	}
	else
		return primary(self, node);

	return 0;
}

static ERR multiplicative(Parser *self, Node *node) {
	ERR err;

	if ((err = unary(self, node)))
		return err;

	while (CURRENT.kind == SlashTk) {
		// Get operator
		Token op = CURRENT;

		NEXT;

		// Get value
		Node right = {};

		if ((err = unary(self, &right)))
			return err;

		// Create node
		if ((err = heap_node(node, &self->diags, &node->binary.left)))
			return err;

		if ((err = heap_node(&right, &self->diags, &node->binary.right)))
			return err;

		node->kind = BinaryNk;
		node->binary.op = op;
	}


	return 0;
}

static ERR additive(Parser *self, Node *node) {
	ERR err;

	if ((err = multiplicative(self, node)))
		return err;

	while (CURRENT.kind == PlusTk || CURRENT.kind == DashTk) {
		// Get operator
		Token op = CURRENT;

		NEXT;

		// Get value
		Node right = {};

		if ((err = multiplicative(self, &right)))
			return err;

		// Create node
		if ((err = heap_node(node, &self->diags, &node->binary.left)))
			return err;

		if ((err = heap_node(&right, &self->diags, &node->binary.right)))
			return err;

		node->kind = BinaryNk;
		node->binary.op = op;
	}

	return 0;
}

static ERR assignment(Parser *self, Node *node) {
	ERR err;

	if ((err = additive(self, node)))
		return err;

	return 0;
}

static ERR parse(Parser *self, Node **node) {
	ERR err;
	NEXT;

	Node *last_node = NULL;
	*node = NULL;

	while (CURRENT.kind != EOFTk) {
		Node new = {};

		if ((err = assignment(self, &new)))
			return err;

		// Linked list append
		Node *in_heap;

		if ((err = heap_node(&new, &self->diags, &in_heap)))
			return err;

		last_node = *node == NULL
			? (*node = in_heap)
			: (last_node->next = in_heap);
	}

	return 0;
}

ERR parser_init(NextFn next, void *next_data, Parser *parser) {
	if (next == NULL || parser == NULL || next == NULL)
		return 1;

	parser->current = new_token(EOFTk, "\0", 0, 1);
	parser->diags = new_diagbag();
	parser->next_data = next_data;

	parser->parse = &parse;
	parser->next = next;

	return 0;
}
