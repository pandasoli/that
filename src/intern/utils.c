#include "lexer/token.h"
#include "parser/node.h"
#include "utils.h"


char *token_strkind(TokenKind kind) {
	switch (kind) {
#define X(kind) case kind ## Tk: return #kind;
		TOKEN_KINDS_LIST(X)
#undef X
	}

	return NULL;
}

char *node_strkind(NodeKind kind) {
	switch (kind) {
#define X(kind) case kind ## Nk: return #kind;
		NODE_KINDS_LIST(X)
#undef X
	}

	return NULL;
}
