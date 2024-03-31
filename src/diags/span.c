#include "diags/span.h"


TextSpan new_textspan(size_t pos, size_t len) {
	return (TextSpan) {
		.pos = pos,
		.len = len,
		.end = pos + len
	};
}
