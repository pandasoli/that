#pragma once

#include "lexer/token.h"
#include "parser/node.h"
#include "diags/bag.h"
#include "globl.h"


typedef Token (*NextFn)(void *data);

typedef struct Parser Parser;
struct Parser {
	Token current;
	DiagBag diags;
	NextFn next;
	void *next_data;

	ERR (*parse)(Parser *self, Node **node);
};

ERR parser_init(NextFn next, void *next_data, Parser *parser);
