#pragma once

#include <thatlang/globl.h>

#include <stdlib.h>


typedef struct thStrView thStrView;
struct thStrView {
	char *data;
	size_t size;
};

thStrView strview_create(char *data, size_t len);

typedef struct thStrBuilder thStrBuilder;
struct thStrBuilder {
	char *data;
	size_t size;
	size_t cap;

	thERR (*append)(thStrBuilder *self, char ch);
	thERR (*append_cstr)(thStrBuilder *self, const char *cstr, size_t len);
	thERR (*extract_view)(thStrBuilder *self, thStrView **view);
	void (*free)(thStrBuilder *self);
};

thERR strbuilder_create(const char *data, size_t size, thStrBuilder *sb);
