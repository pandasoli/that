#include "lexer/token.h"
#include "diags/span.h"


Token new_token(TokenKind kind, char *literal, size_t pos, size_t len) {
	Token res;

	res.kind = kind;
	res.literal = literal;
	res.span = new_textspan(pos, len);

	return res;
}
