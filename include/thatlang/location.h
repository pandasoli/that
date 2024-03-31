#pragma once

#include <stddef.h>


typedef struct {
	size_t
		pos,
		len,
		end;
} thLocation;

thLocation th_location_create(size_t pos, size_t len);
