#include <thatlang/lexer/token.h>
#include <thatlang/parser/parser.h>
#include <intern/parser/node.h>
#include <intern/diag_bag.h>
#include <intern/utils.h>

#include <stdio.h>
#include <assert.h>

#define CURRENT self->current
#define NEXT ERR(self->next(self->next_props, &CURRENT))


/* Precedence order
 *   - Primary
 *   - Unary
 *   - Multiplicative
 *   - Additive
 *   - Comparison
 *   - Logical
 * ^ - Function
 * | - Member
 * | - Declare
 * | - Assignment
 *
 * I decided to keep addition and multiplication separated for better understanding
 * Not like https://github.com/pandasoli/minsk/blob/master/Minsk/CodeAnalysis/Syntax/Parser.cs#L86
 */

static thERR expr(thParser *self, thNode **node);
static thERR primary(thParser *self, thNode **node);

static thERR process_type(thParser *self, thTypeNode **node) {
	thERR err;

	*node = NULL;
	thTypeNode *last_node;
	thTypeNode *new = NULL;

	while (1) {
		switch (CURRENT.kind) {
			case thIdentifierTk:
				ERR(node_identifier_create(CURRENT, &new));
				NEXT;
				break;

			case thFnKw:
				NEXT;

				thTypeNode *type;

				// TODO: what if it's nothing?
				// Add NEXT if it's nothing
				ERR(process_type(self, &type));
				ERR(node_fn_create(type, NULL, &new));
				break;

			default:
				report(CURRENT.location, "%s isn't identified as a type", token_strkind(CURRENT.kind));
				return 2;
		}

		// Linked list append
		last_node = *node == NULL
			? (*node = new)
			: (last_node->next = new);

		if (CURRENT.kind != thPipeTk)
			break;
		NEXT;
	}

	return 0;
}

static thERR primary(thParser *self, thNode **node) {
	thERR err;

	switch (CURRENT.kind) {
		case thFloatTk:
		case thIntTk:
			ERR(node_number_create(CURRENT, node));
			NEXT;
			break;
		
		case thIdentifierTk:
			ERR(node_identifier_create(CURRENT, node));
			NEXT;
			break;

		case thFnKw:
			NEXT;

			// Get return type
			thTypeNode *ret_type;

			ERR(process_type(self, &ret_type));

			// Get body
			thNode *body = NULL;

			if (CURRENT.kind == thOpenBraceTk) {
				NEXT;

				ERR(expr(self, &body));

				if (CURRENT.kind != thCloseBraceTk) {
					report(CURRENT.location, "Found %s while looking for the end of fn's body", token_strkind(CURRENT.kind));
					return 2;
				}
			}

			// Make node
			ERR(node_fn_create(ret_type, body, node));
			break;

		case thOpenBraceTk:
			NEXT;

			ERR(expr(self, node));

			if (CURRENT.kind != thCloseBraceTk) {
				report(CURRENT.location, "Expected closing parenthesis when found %s", token_strkind(CURRENT.kind));
				return 2;
			}

			NEXT;
			break;

		default:
			report(CURRENT.location, "Unexpected %s token found parsing primary", token_strkind(CURRENT.kind));
			return 2;
	}

	return 0;
}

static thERR unary(thParser *self, thNode **node) {
	thERR err;

	if (CURRENT.kind == thDashTk)
	{
		thToken op = CURRENT;
		NEXT;

		thNode *value;

		ERR(primary(self, &value));
		ERR(node_unary_create(op, value, node));
	}
	else
		return primary(self, node);

	return 0;
}

static thERR mul(thParser *self, thNode **node) {
	thERR err;

	ERR(unary(self, node));

	while (CURRENT.kind == thSlashTk) {
		thToken op = CURRENT;
		NEXT;

		thNode *right;

		ERR(unary(self, &right));
		ERR(node_binary_create(*node, op, right, node));
	}

	return 0;
}

static thERR add(thParser *self, thNode **node) {
	thERR err;

	ERR(mul(self, node));

	while (CURRENT.kind == thPlusTk || CURRENT.kind == thDashTk) {
		thToken op = CURRENT;
		NEXT;

		thNode *right;

		ERR(mul(self, &right));
		ERR(node_binary_create(*node, op, right, node));
	}

	return 0;
}

static thERR cmp(thParser *self, thNode **node) {
	thERR err;

	ERR(add(self, node));

	while (
		CURRENT.kind == thLessThanTk || CURRENT.kind == thGreaterThanTk ||
		CURRENT.kind == thEqualsToTk || CURRENT.kind == thDiffTk)
	{
		thToken op = CURRENT;
		NEXT;

		thNode *right;

		ERR(add(self, &right));
		ERR(node_binary_create(*node, op, right, node));
	}

	return 0;
}

static thERR logical(thParser *self, thNode **node) {
	thERR err;

	ERR(cmp(self, node));

	while (CURRENT.kind == thAndKw || CURRENT.kind == thOrKw) {
		thToken op = CURRENT;
		NEXT;

		thNode *right;

		ERR(cmp(self, &right));
		ERR(node_binary_create(*node, op, right, node));
	}

	return 0;
}

static thERR decl(thParser *self, thNode **node) {
	thERR err;

	if (CURRENT.kind == thLocalKw) {
		NEXT;

		// Get name
		thToken name = CURRENT;
		NEXT;

		if (name.kind != thIdentifierTk) {
			report(CURRENT.location, "Variable name expected to be an indentifier");
			return 2;
		}

		// Get type
		thTypeNode *type;

		ERR(process_type(self, &type));

		if (type == NULL) {
			report(CURRENT.location, "Expected variable type after name");
			return 2;
		}

		// Create node
		ERR(node_assign_create(name, type, NULL, node));
	}
	else
		return logical(self, node);

	return 0;
}

static thERR assign(thParser *self, thNode **node) {
	thERR err;

	ERR(decl(self, node));

	while (CURRENT.kind == thEqualsTk) {
		// Validate left
		if ((*node)->kind != thAssignNk) {
			report(CURRENT.location, "Cannot assign to %s operation", node_strkind((*node)->kind));
			return 2;
		}

		// Get op
		thToken op = CURRENT;
		NEXT;

		// Get right
		thNode *right;

		ERR(decl(self, &right));
		ERR(node_binary_create(*node, op, right, node));
	}


	return 0;
}

/* Just a short for the toppest expr */
static thERR expr(thParser *self, thNode **node) {
	return assign(self, node);
}

static thERR parse(thParser *self, thNode **node) {
	assert(self != NULL);
	assert(node != NULL);

	thERR err;
	NEXT;

	thNode *last_node = NULL;
	*node = NULL;

	while (CURRENT.kind != thEOFTk) {
		thNode *new;

		ERR(expr(self, &new));

		// Linked list append
		last_node = *node == NULL
			? (*node = new)
			: (last_node->next = new);
	}

	return 0;
}

thERR th_parser_init(thNextFn next, void *next_props, thParser *parser) {
	assert(next != NULL);
	assert(parser != NULL);

	parser->current = (thToken) {};
	parser->next = next;
	parser->next_props = next_props;

	parser->parse = &parse;

	return 0;
}
