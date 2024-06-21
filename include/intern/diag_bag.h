#pragma once

#include <thatlang/diags/bag.h>


thERR report_intern(char *format, ...);
thERR report(thLocation location, char *format, ...);
