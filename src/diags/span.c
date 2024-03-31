#include <thatlang/diags/span.h>


thTextSpan th_textspan_create(size_t pos, size_t len) {
	return (thTextSpan) {
		.pos = pos,
		.len = len,
		.end = pos + len
	};
}
