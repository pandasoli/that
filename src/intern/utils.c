#include <thatlang/lexer/token.h>
#include <thatlang/parser/node.h>
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

char *node_strkind(thNodeKind kind) {
	switch (kind) {
#define X(kind) case th ## kind ## Nk: return #kind;
	TH_NODE_KINDS_LIST(X)
#undef X
	}

	return NULL;
}
