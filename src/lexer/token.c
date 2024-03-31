#include <thatlang/lexer/token.h>
#include <thatlang/location.h>


thToken th_token_create(thTokenKind kind, char *literal, size_t pos, size_t len) {
	return (thToken) {
		.kind = kind,
		.literal = literal,
		.location = th_location_create(pos, len)
	};
}
