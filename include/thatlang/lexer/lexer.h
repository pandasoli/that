#pragma once

#include <thatlang/lexer/token.h>
#include <thatlang/globl.h>


typedef struct thLexer thLexer;
struct thLexer {
	int fd;
	char current;
	size_t pos;

	thERR (*lex)(thLexer *self, thToken *token);
	thERR (*next)(thLexer *self);
};

typedef struct {
	thTokenKind kind;
	char *image;
} thKeywordInfo;

thERR th_lexer_init(int fd, thLexer *lexer);
