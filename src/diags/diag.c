#include <stdio.h>
#include <stdlib.h>
#include "diags/bag.h"
#include "diags/diag.h"


static void free_(Diag *self) {
	free(self->msg);
	free(self);
}

ERR new_diag(TextSpan span, char *msg, DiagBag *bag, Diag **diag) {
	Diag *res = malloc(sizeof(Diag));

	if (res == NULL) {
		bag->report_intern(bag, "malloc(%zu) returned NULL on new_diag", sizeof(Diag));
		return 2;
	}

	*res = (Diag) {
		.span = span,
		.msg = msg,
		.free = &free_,
		.next = NULL
	};

	*diag = res;

	return 0;
}
