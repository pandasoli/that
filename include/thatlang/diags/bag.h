#pragma once

#include <thatlang/diags/diag.h>
#include <thatlang/location.h>
#include <thatlang/globl.h>


typedef struct thDiagBag thDiagBag;
struct thDiagBag {
	thDiag *diags;
	thDiag *intern;

	void (*free)(thDiagBag *self);

	thERR (*report_intern)(thDiagBag *self, char *format, ...);
	thERR (*report)(thDiagBag *self, thLocation location, char *format, ...);
};

thDiagBag th_diagbag_create();
