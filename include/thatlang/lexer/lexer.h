#pragma once

#include "token.h"
#include "globl.h"


typedef struct Lexer Lexer;
struct Lexer {
	char *text;
	size_t pos;

	Token (*lex)(Lexer *self);
};

ERR lexer_init(char *text, Lexer *lexer);
