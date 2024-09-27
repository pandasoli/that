#include <thatlang/lexer/token.h>
#include <intern/utils.h>


char *token_strkind(thTokenKind kind) {
	switch (kind) {
#define X(kind)       case th ## kind ## Tk: return #kind;
#define KW(kind, ...) case th ## kind ## Kw: return #kind;
	TH_TOKEN_KINDS_LIST(X, KW)
#undef X
#undef KW
	}

	return NULL;
}
