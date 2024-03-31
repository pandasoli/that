#include <thatlang/lexer/token.h>
#include <intern/utils.h>


char *token_strkind(thTokenKind kind) {
	switch (kind) {
#define X(kind) case kind ## Tk: return #kind;
		TOKEN_KINDS_LIST(X)
#undef X
	}

	return NULL;
}
