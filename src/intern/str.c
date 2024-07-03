#include <intern/diag_bag.h>
#include <intern/str.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


static thERR append_cstr(thStrBuilder *self, char *cstr, size_t len) {
	assert(self != NULL);
	assert(cstr != NULL);

	if (self->size + len + 1 > self->cap) {
		if (self->cap == 0) self->cap++;

		while (self->size + len + 1 > self->cap)
			self->cap *= 2;

		self->data = realloc(self->data, self->cap);

		if (self->data == NULL) {
			report_intern("realloc(%zu) returned NULL on thStrBuilder.append_cstr", self->cap);
			return 2;
		}
	}

	strncpy(self->data + self->size, cstr, len);
	self->data[self->size + len + 1] = '\0';
	self->size += len;

	return 0;
}

static thERR append(thStrBuilder *self, char ch) {
	assert(self != NULL);
	return append_cstr(self, &ch, 1);
}

static thERR extract_view(thStrBuilder *self, thStrView **view) {
	thStrView *res = malloc(sizeof *res);
	if (res == NULL) {
		report_intern("malloc(%zu) returned NULL on thStrBuilder.extract_view", sizeof *res);
		return 2;
	}

	char *data = malloc(self->size);
	if (data == NULL) {
		report_intern("malloc(%zu) returned NULL on thStrBuilder.extract_view for data", self->size);
		return 2;
	}

	strncpy(data, self->data, self->size);

	res->data = data;
	res->size = self->size;

	*view = res;
	return 0;
}

static void free_(thStrBuilder *self) {
	free(self->data);
}

thERR strbuilder_create(char *data, size_t len, thStrBuilder *sb) {
	thStrBuilder res = {
		.data = NULL,
		.size = 0,
		.cap = 0,
		.append = &append,
		.append_cstr = &append_cstr,
		.extract_view = &extract_view,
		.free = &free_
	};

	append_cstr(&res,
		data == NULL ? "" : data,
		data == NULL ? 0 : len
	);

	*sb = res;

	return 0;
}

thStrView strview_create(char *data, size_t size) {
	return (thStrView) {
		.data = data,
		.size = size
	};
}
