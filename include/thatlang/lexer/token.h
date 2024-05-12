#pragma once

#include <thatlang/location.h>

#include <stddef.h>

#define TOKEN_KINDS_LIST(T, KW) \
		T(Unknown)                  \
		T(EOF)                      \
															  \
		/* Symbols */               \
		T(Plus) T(Dash) T(Slash)    \
		T(Comma)                    \
		T(OpenBrace) T(CloseBrace)  \
		T(OpenParen) T(CloseParen)  \
															  \
		T(LessThan) T(GreaterThan)  \
		T(EqualsTo) T(Diff)         \
															  \
		T(Ampersand) T(Asterisk)    \
		T(Pipe) T(Equals)           \
															  \
		/* Literals */              \
		T(Identifier)               \
		T(Int) T(Float)             \
															  \
		/* Keywords */              \
		KW(Import, "import")        \
		KW(Pub, "pub")              \
		KW(Local, "local")          \
		KW(Type, "type")            \
		KW(As, "as")                \
		KW(Not, "not")              \
		KW(And, "and")              \
		KW(Or, "or")                \
		KW(Fn, "fn")                \
		KW(Return, "return")

typedef enum thTokenKind thTokenKind;
enum thTokenKind {
#define T(kind) kind ## Tk,
#define KW(kind, ...) kind ## Kw,
	TOKEN_KINDS_LIST(T, KW)
#undef T
#undef KW
};

typedef struct thToken thToken;
struct thToken {
	thTokenKind kind;
	char *literal;
	thLocation location;
};

thToken th_token_create(thTokenKind kind, char *literal, size_t pos, size_t len);
