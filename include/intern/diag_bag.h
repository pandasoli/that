#pragma once

#include <thatlang/diags/bag.h>

#define ERR(c)              \
	if ((err = c)) return err


thERR report_intern(const char *format, ...);
thERR report(thLocation location, const char *format, ...);
