#pragma once

#include <thatlang/location.h>
#include <thatlang/globl.h>


typedef struct th_DiagBag thDiagBag;

typedef struct th_Diag thDiag;
struct th_Diag {
	thLocation location;
	char *msg;

	void (*free)(thDiag *self);

	thDiag *next;
};

thERR th_diag_create(thLocation location, char *msg, thDiagBag *bag, thDiag **diag);
