#pragma once

#include <stddef.h>


typedef struct {
	size_t
		pos,
		len,
		end;
} thTextSpan;

thTextSpan th_textspan_create(size_t pos, size_t len);
