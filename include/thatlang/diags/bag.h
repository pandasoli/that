#pragma once

#include <thatlang/diags/diag.h>
#include <thatlang/location.h>
#include <thatlang/globl.h>


/* 0 - Sucess
 * 1 - Error on libc function
 * 2 - Syntax error on parsing
 */
typedef unsigned char thERR;

typedef struct thDiagBag thDiagBag;
struct thDiagBag {
	thDiag *diags;
	thDiag *intern;

	void (*free)();
};

extern thDiagBag th_diags;
