#include <thatlang/diags/bag.h>
#include <thatlang/diags/diag.h>
#include <intern/diag_bag.h>

#include <stdio.h>
#include <stdlib.h>


static void free_(thDiag *self) {
	free(self->msg);
	free(self);
}

thERR th_diag_create(thLocation location, char *msg, thDiag **diag) {
	thDiag *res = malloc(sizeof *res);

	if (res == NULL) {
		report_intern("malloc(%zu) returned NULL on new_diag", sizeof(thDiag));
		return 2;
	}

	*res = (thDiag) {
		.location = location,
		.msg = msg,
		.free = &free_,
		.next = NULL
	};

	*diag = res;

	return 0;
}
