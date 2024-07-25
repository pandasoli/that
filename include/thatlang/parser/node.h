#pragma once

#include <thatlang/lexer/token.h>

#define NODE_KINDS_LIST(N) \
	N(Binary)                \
	N(Unary)                 \
	N(Number)


typedef enum thNodeKind thNodeKind;
enum thNodeKind {
#define N(kind) kind ## Nk,
	NODE_KINDS_LIST(N)
#undef N
};

typedef struct thNode thNode;

typedef thToken thNumberNode;

typedef struct thBinaryNode thBinaryNode;
struct thBinaryNode {
	thNode *left;
	thToken op;
	thNode *right;
};

typedef struct thUnaryNode thUnaryNode;
struct thUnaryNode {
	thToken op;
	thNode *val;
};

struct thNode {
	thNodeKind kind;

	union {
		thBinaryNode binary;
		thUnaryNode unary;
		thNumberNode number;
	};

	thNode *next;
};
