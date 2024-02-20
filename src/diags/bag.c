#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "diags/span.h"
#include "diags/bag.h"
#include "diags/diag.h"
#include "globl.h"


static ERR formatstr(char *format, va_list argv, DiagBag *bag, char **str) {
	va_list argv_copy;
	va_copy(argv_copy, argv);

	int size = vsnprintf(NULL, 0, format, argv) + 1;

	char *res = malloc(size);
	if (res == NULL) {
		bag->report_intern(bag, "malloc(%zu) returned NULL on DiagBag.formatstr.", size);
		return 2;
	}

	vsnprintf(res, size, format, argv_copy);

	va_end(argv);
	va_end(argv_copy);

	*str = res;

	return 0;
}

void append(Diag *diag, Diag **to) {
	if (*to == NULL) {
		*to = diag;
	}
	else {
		Diag *current = *to;
		while (current->next != NULL)
			current = current->next;
		current->next = diag;
	}
}

static void free_(DiagBag *self) {
	Diag *current = self->diags;

	while (current != NULL) {
		Diag *next = current->next;

		current->free(current);
		current = next;
	}
}

static ERR report_intern(DiagBag *self, char *format, ...) {
	va_list argv;
	va_start(argv, format);

	ERR err;
	Diag *diag;
	char *msg;
	TextSpan span = {};

	// Format message
	err = formatstr(format, argv, self, &msg);
	if (err > 0) return err;

	// Create diagnostic
	err = new_diag(span, msg, self, &diag);
	if (err > 0) return err;

	append(diag, &self->intern);

	return 0;
}

static ERR report(DiagBag *self, TextSpan span, char *format, ...) {
	va_list argv;
	va_start(argv, format);

	ERR err;
	Diag *diag;
	char *msg;

	// Format message
	err = formatstr(format, argv, self, &msg);
	if (err > 0) return err;

	// Create diagnostic
	err = new_diag(span, msg, self, &diag);
	if (err > 0) return err;

	append(diag, &self->diags);

	return 0;
}

DiagBag new_diagbag() {
	DiagBag res;

	res.intern = NULL;
	res.diags = NULL;

	res.free = &free_;

	res.report_intern = &report_intern;
	res.report = &report;

	return res;
}
