#pragma once

#include <thatlang/lexer/token.h>
#include <thatlang/globl.h>


typedef struct thLexer thLexer;
struct thLexer {
	char *text;
	size_t pos;

	thToken (*lex)(thLexer *self);
};

thERR th_lexer_init(char *text, thLexer *lexer);
