#include <thatlang/diags/bag.h>
#include <thatlang/diags/diag.h>
#include <thatlang/location.h>
#include <thatlang/globl.h>
#include <intern/diag_bag.h>

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>


static void free_() {
	thDiag *current = th_diags.diags;

	while (current != NULL) {
		thDiag *next = current->next;

		current->free(current);
		current = next;
	}
}

static thERR report_to(thDiag **to, thLocation location, const char *format, va_list args) {
	assert(to != NULL);

	thERR err;
	thDiag *diag;
	char *msg;

	// Format message
	if (vasprintf(&msg, format, args) < 0) {
		report_intern("vasprintf returned -1 on report_to.");
		return 1;
	}

	// Create diagnostic
	ERR(th_diag_create(location, msg, &diag));

	// Append to the list
	if (*to == NULL) {
		*to = diag;
	}
	else {
		thDiag *current = *to;
		while (current->next != NULL)
			current = current->next;
		current->next = diag;
	}

	return 0;
}

thERR report_intern(const char *format, ...) {
	assert(format != NULL);

	va_list args;
	va_start(args, format);
	thERR err = report_to(&th_diags.intern, (thLocation) {}, format, args);

	va_end(args);
	return err;
}

thERR report(thLocation location, const char *format, ...) {
	assert(format != NULL);

	va_list args;
	va_start(args, format);
	thERR err = report_to(&th_diags.diags, location, format, args);

	va_end(args);
	return err;
}

thDiagBag th_diags = (thDiagBag) {
	.intern = NULL,
	.diags = NULL,

	.free = &free_
};
