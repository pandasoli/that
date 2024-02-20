#pragma once

#include "diags/diag.h"
#include "diags/span.h"
#include "globl.h"


typedef struct DiagBag DiagBag;
struct DiagBag {
	Diag *diags;
	Diag *intern;

	void (*free)(DiagBag *self);

	ERR (*report_intern)(DiagBag *self, char *format, ...);
	ERR (*report)(DiagBag *self, TextSpan span, char *format, ...);
};

DiagBag new_diagbag();
