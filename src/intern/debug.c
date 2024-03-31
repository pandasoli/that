#include <thatlang/lexer/token.h>
#include <intern/debug.h>
#include <intern/utils.h>

#include <stdio.h>


void print_token(thToken token) {
	printf(
		"%s \"%.*s\" %zu %zu",
		token_strkind(token.kind),
		(int) token.location.len, token.literal,
		token.location.pos,
		token.location.len
	);
}
