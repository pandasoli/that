#include <thatlang/lexer/token.h>
#include <intern/utils.h>


char *token_strkind(thTokenKind kind) {
	switch (kind) {
#define X(kind)       case kind ## Tk: return #kind;
#define KW(kind, ...) case kind ## Kw: return #kind;
		TOKEN_KINDS_LIST(X, KW)
#undef X
#undef KW
	}

	return NULL;
}
