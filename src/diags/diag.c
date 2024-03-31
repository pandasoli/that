#include <thatlang/diags/bag.h>
#include <thatlang/diags/diag.h>

#include <stdio.h>
#include <stdlib.h>


static void free_(thDiag *self) {
	free(self->msg);
	free(self);
}

thERR th_diag_create(thTextSpan span, char *msg, thDiagBag *bag, thDiag **diag) {
	thDiag *res = malloc(sizeof(thDiag));

	if (res == NULL) {
		bag->report_intern(bag, "malloc(%zu) returned NULL on new_diag", sizeof(thDiag));
		return 2;
	}

	*res = (thDiag) {
		.span = span,
		.msg = msg,
		.free = &free_,
		.next = NULL
	};

	*diag = res;

	return 0;
}
