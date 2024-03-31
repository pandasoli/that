#include "lexer/token.h"
#include "diags/span.h"


Token new_token(TokenKind kind, char *literal, size_t pos, size_t len) {
	return (Token) {
		.kind = kind,
		.literal = literal,
		.span = new_textspan(pos, len)
	};
}
