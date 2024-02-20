#pragma once

#include "diags/span.h"
#include "globl.h"


typedef struct DiagBag DiagBag;

typedef struct Diag Diag;
struct Diag {
	TextSpan span;
	char *msg;

	void (*free)(Diag *self);

	Diag *next;
};

ERR new_diag(TextSpan span, char *msg, DiagBag *bag, Diag **diag);
