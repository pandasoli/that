#include "diags/span.h"


TextSpan new_textspan(size_t pos, size_t len) {
	TextSpan res;

	res.pos = pos;
	res.len = len;
	res.end = pos + len;

	return res;
}
