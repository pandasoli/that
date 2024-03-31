#include <thatlang/diags/bag.h>
#include <thatlang/diags/diag.h>
#include <thatlang/location.h>
#include <thatlang/globl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


static thERR formatstr(char *format, va_list argv, thDiagBag *bag, char **str) {
	va_list argv_copy;
	va_copy(argv_copy, argv);

	int size = vsnprintf(NULL, 0, format, argv) + 1;

	char *res = malloc(size);
	if (res == NULL) {
		bag->report_intern(bag, "malloc(%zu) returned NULL on th_DiagBag.formatstr.", size);
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

static void free_(thDiagBag *self) {
	thDiag *current = self->diags;

	while (current != NULL) {
		thDiag *next = current->next;

		current->free(current);
		current = next;
	}
}

static thERR report_intern(thDiagBag *self, char *format, ...) {
	va_list argv;
	va_start(argv, format);

	thERR err;
	thDiag *diag;
	char *msg;
	thLocation location = {};

	// Format message
	err = formatstr(format, argv, self, &msg);
	if (err > 0) return err;

	// Create diagnostic
	err = th_diag_create(location, msg, self, &diag);
	if (err > 0) return err;

	append(diag, &self->intern);

	return 0;
}

static thERR report(thDiagBag *self, thLocation location, char *format, ...) {
	va_list argv;
	va_start(argv, format);

	thERR err;
	thDiag *diag;
	char *msg;

	// Format message
	err = formatstr(format, argv, self, &msg);
	if (err > 0) return err;

	// Create diagnostic
	err = th_diag_create(location, msg, self, &diag);
	if (err > 0) return err;

	append(diag, &self->diags);

	return 0;
}

thDiagBag th_diagbag_create() {
	return (thDiagBag) {
		.intern = NULL,
		.diags = NULL,

		.free = &free_,

		.report_intern = &report_intern,
		.report = &report
	};
}
