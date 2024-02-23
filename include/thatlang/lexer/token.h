#pragma once

#include <stddef.h>
#include "diags/span.h"


typedef enum TokenKind TokenKind;
enum TokenKind {
	UnknownTk,
	EOFTk,

	// Symbols
	PlusTk, DashTk, SlashTk,

	CommaTk,
	OpenBraceTk, CloseBraceTk,
	OpenParenTk, CloseParenTk,

	LessThanTk, GreaterThanTk,
	EqualsToTk, DiffTk,

	AmpersandTk, AsteriskTk,
	PipeTk, EqualsTk,

	// Literals
	IdentifierTk,
	IntTk, FloatTk,

	// Keywords
	ImportTk, PubTk,
	LocalTk, TypeTk, AsTk,
	NotTk,
	AndTk, OrTk,
	FnTk, ReturnTk
};

typedef struct Token Token;
struct Token {
	TokenKind kind;
	char *literal;
	TextSpan span;
};

Token new_token(TokenKind kind, char *literal, size_t pos, size_t len);
