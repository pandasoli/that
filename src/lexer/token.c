#include <thatlang/lexer/token.h>
#include <thatlang/location.h>
#include <intern/str.h>


thToken th_token_create(thTokenKind kind, const char *literal, size_t pos, size_t len) {
	return (thToken) {
		.kind = kind,
		.literal = literal,
		.location = th_location_create(pos, len)
	};
}
