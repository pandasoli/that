#include <thatlang/diags/bag.h>
#include <thatlang/diags/diag.h>
#include <thatlang/location.h>
#include <thatlang/globl.h>
#include <intern/diag_bag.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


static thERR formatstr(char *format, va_list argv, char **str) {
	va_list argv_copy;
	va_copy(argv_copy, argv);

	int size = vsnprintf(NULL, 0, format, argv) + 1;

	char *res = malloc(size);
	if (res == NULL) {
		report_intern("malloc(%zu) returned NULL on th_DiagBag.formatstr.", size);
		return 2;
	}

	vsnprintf(res, size, format, argv_copy);

	va_end(argv);
	va_end(argv_copy);

	*str = res;

	return 0;
}

void append(thDiag *diag, thDiag **to) {
	if (*to == NULL) {
		*to = diag;
	}
	else {
		thDiag *current = *to;
		while (current->next != NULL)
			current = current->next;
		current->next = diag;
	}
}

static void free_() {
	thDiag *current = th_diags.diags;

	while (current != NULL) {
		thDiag *next = current->next;

		current->free(current);
		current = next;
	}
}

thERR report_intern(char *format, ...) {
	va_list argv;
	va_start(argv, format);

	thERR err;
	thDiag *diag;
	char *msg;
	thLocation location = {};

	// Format message
	err = formatstr(format, argv, &msg);
	if (err > 0) return err;

	// Create diagnostic
	err = th_diag_create(location, msg, &diag);
	if (err > 0) return err;

	append(diag, &th_diags.intern);

	return 0;
}

thERR report(thLocation location, char *format, ...) {
	va_list argv;
	va_start(argv, format);

	thERR err;
	thDiag *diag;
	char *msg;

	// Format message
	err = formatstr(format, argv, &msg);
	if (err > 0) return err;

	// Create diagnostic
	err = th_diag_create(location, msg, &diag);
	if (err > 0) return err;

	append(diag, &th_diags.diags);

	return 0;
}

thDiagBag th_diags = (thDiagBag) {
	.intern = NULL,
	.diags = NULL,

	.free = &free_
};
