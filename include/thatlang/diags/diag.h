#pragma once

#include <thatlang/diags/span.h>
#include <thatlang/globl.h>


typedef struct th_DiagBag thDiagBag;

typedef struct th_Diag thDiag;
struct th_Diag {
	thTextSpan span;
	char *msg;

	void (*free)(thDiag *self);

	thDiag *next;
};

thERR th_diag_create(thTextSpan span, char *msg, thDiagBag *bag, thDiag **diag);
