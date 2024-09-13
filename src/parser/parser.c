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

	switch (CURRENT.kind) {
		case thIdentifierTk:
			ERR(node_identifier_create(CURRENT, node));
			NEXT;
			break;

		case thFnKw:
			NEXT;

			// Get args
			thArgs *args = NULL;
			thArgs *last_arg = NULL;

			if (CURRENT.kind == thOpenParenTk) {
				NEXT;

				while (CURRENT.kind != thCloseParenTk) {
					thTypeNode *type;
					thArgs *arg;

					ERR(process_type(self, &type));
					ERR(node_args_create((thToken) {}, type, &arg));

					// Linked list append
					last_arg = args == NULL
						? (args = arg)
						: (last_arg->next = arg);

					if (CURRENT.kind == thCommaTk) {
						NEXT;
					}
					else if (CURRENT.kind != thCloseParenTk) {
						report(CURRENT.location, "Found %s while looking for comma or fn args end", token_strkind(CURRENT.kind));
						return 2;
					}
				}

				if (CURRENT.kind != thCloseParenTk) {
					report(CURRENT.location, "Found %s while looking for fn args end", token_strkind(CURRENT.kind));
					return 2;
				}
				NEXT;
			}

			// Get return type
			thTypeNode *ret = NULL;

			ERR(process_type(self, &ret));

			// Make node
			ERR(node_fn_create(args, ret, NULL, node));
			break;

		default: {}
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

		case thAmpersandTk:
		case thAsteriskTk: {
			char deref = CURRENT.kind == thAsteriskTk;
			NEXT;

			thToken identifier = CURRENT;
			NEXT;

			if (identifier.kind != thIdentifierTk) {
				report(CURRENT.location, "A %s doesn't have an address, only identifiers can be (de)referenced", token_strkind(identifier.kind));
				return 2;
			}

			ERR(node_reference_create(deref, identifier, node));
		} break;

		case thFnKw:
			NEXT;

			// Get args
			thArgs *args = NULL;
			thArgs *last_arg = NULL;

			if (CURRENT.kind == thOpenParenTk) {
				NEXT;

				while (CURRENT.kind != thCloseParenTk) {
					thToken indentifier;

					if (CURRENT.kind != thIdentifierTk) {
						report(CURRENT.location, "Found %s while looking for fn arg name", token_strkind(CURRENT.kind));
						return 2;
					}
					else
						indentifier = CURRENT;
					NEXT;

					thTypeNode *type;
					thArgs *arg;

					ERR(process_type(self, &type));
					ERR(node_args_create(indentifier, type, &arg));

					// Linked list append
					last_arg = args == NULL
						? (args = arg)
						: (last_arg->next = arg);

					if (CURRENT.kind == thCommaTk) {
						NEXT;
					}
					else if (CURRENT.kind != thCloseParenTk) {
						report(CURRENT.location, "Found %s while looking for comma or fn args end", token_strkind(CURRENT.kind));
						return 2;
					}
				}

				if (CURRENT.kind != thCloseParenTk) {
					report(CURRENT.location, "Found %s while looking for fn args end", token_strkind(CURRENT.kind));
					return 2;
				}
				NEXT;
			}

			// Get return type
			thTypeNode *ret = NULL;

			ERR(process_type(self, &ret));

			// Get body
			thNode *body = NULL;

			if (CURRENT.kind == thOpenBraceTk) {
				NEXT;

				ERR(expr(self, &body));

				if (CURRENT.kind != thCloseBraceTk) {
					report(CURRENT.location, "Found %s while looking for the end of fn's body", token_strkind(CURRENT.kind));
					return 2;
				}
				NEXT;
			}

			// Make node
			ERR(node_fn_create(args, ret, body, node));
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
		thTokenKind op = CURRENT.kind;
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
		thTokenKind op = CURRENT.kind;
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
		thTokenKind op = CURRENT.kind;
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
		thTokenKind op = CURRENT.kind;
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
		thTokenKind op = CURRENT.kind;
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
		thTokenKind op = CURRENT.kind;
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
