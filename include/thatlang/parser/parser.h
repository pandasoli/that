#pragma once

#include <thatlang/lexer/token.h>
#include <thatlang/parser/node.h>


typedef thERR (*thNextFn)(void *props, thToken *token);

typedef struct thParser thParser;
struct thParser {
	thToken current;
	thNextFn next;
	void *next_props;

	thERR (*parse)(thParser *self, thNode **node);
};

thERR th_parser_init(thNextFn next, void *next_props, thParser *parser);
