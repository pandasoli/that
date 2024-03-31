#include <thatlang/lexer/token.h>
#include <thatlang/diags/span.h>


thToken th_token_create(thTokenKind kind, char *literal, size_t pos, size_t len) {
	return (thToken) {
		.kind = kind,
		.literal = literal,
		.span = th_textspan_create(pos, len)
	};
}
