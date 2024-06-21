#pragma once

#include <thatlang/location.h>
#include <thatlang/globl.h>


typedef struct thDiagBag thDiagBag;

typedef struct thDiag thDiag;
struct thDiag {
	thLocation location;
	char *msg;

	void (*free)(thDiag *self);

	thDiag *next;
};

thERR th_diag_create(thLocation location, char *msg, thDiagBag *bag, thDiag **diag);
