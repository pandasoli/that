#pragma once

#include <stddef.h>
#include "diags/span.h"


typedef enum TokenKind TokenKind;
enum TokenKind {
	UnknownTk,
	EOFTk,

	// Symbols
	PlusTk, DashTk,
	AsteriskTk, SlashTk,
	OpenParenTk, CloseParenTk,
	EqualsTk,

	// Literal values
	IdentifierTk,
	IntTk, FloatTk
};

typedef struct Token Token;
struct Token {
	TokenKind kind;
	char *literal;
	TextSpan span;
};

Token new_token(TokenKind kind, char *literal, size_t pos, size_t len);
