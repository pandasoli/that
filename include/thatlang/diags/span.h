#pragma once

#include <stddef.h>


typedef struct {
	size_t
		pos,
		len,
		end;
} TextSpan;

TextSpan new_textspan(size_t pos, size_t len);
