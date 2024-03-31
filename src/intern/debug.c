#include <thatlang/lexer/token.h>
#include <intern/debug.h>
#include <intern/utils.h>

#include <stdio.h>


void print_token(thToken token) {
	printf(
		"%s \"%.*s\" %zu %zu",
		token_strkind(token.kind),
		(int) token.span.len, token.literal,
		token.span.pos,
		token.span.len
	);
}
