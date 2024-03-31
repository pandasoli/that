#pragma once

#include <thatlang/diags/span.h>

#include <stddef.h>

#define TOKEN_KINDS_LIST(X)    \
		X(Unknown)                 \
		X(EOF)                     \
															 \
		/* Symbols */              \
		X(Plus) X(Dash) X(Slash)   \
		X(Comma)                   \
		X(OpenBrace) X(CloseBrace) \
		X(OpenParen) X(CloseParen) \
															 \
		X(LessThan) X(GreaterThan) \
		X(EqualsTo) X(Diff)        \
															 \
		X(Ampersand) X(Asterisk)   \
		X(Pipe) X(Equals)          \
															 \
		/* Literals */             \
		X(Identifier)              \
		X(Int) X(Float)            \
															 \
		/* Keywords */             \
		X(Import) X(Pub)           \
		X(Local) X(Type) X(As)     \
		X(Not)                     \
		X(And) X(Or)               \
		X(Fn) X(Return)

typedef enum thTokenKind thTokenKind;
enum thTokenKind {
#define X(kind) kind ## Tk,
	TOKEN_KINDS_LIST(X)
#undef X
};

typedef struct thToken thToken;
struct thToken {
	thTokenKind kind;
	char *literal;
	thTextSpan span;
};

thToken th_token_create(thTokenKind kind, char *literal, size_t pos, size_t len);
