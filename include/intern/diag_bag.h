#pragma once

#include <thatlang/diags/bag.h>


thERR report_intern(const char *format, ...);
thERR report(thLocation location, const char *format, ...);
