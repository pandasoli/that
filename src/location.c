#include <thatlang/location.h>


thLocation th_location_create(size_t pos, size_t len) {
	return (thLocation) {
		.pos = pos,
		.len = len,
		.end = pos + len
	};
}
