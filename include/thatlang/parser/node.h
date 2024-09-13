#pragma once

#include <thatlang/lexer/token.h>

#define TH_NODE_KINDS_LIST(N) \
	N(Number)                   \
	N(Identifier)               \
	N(Reference)                \
	N(Dereference)              \
	N(Fn)                       \
	N(Binary)                   \
	N(Unary)                    \
	N(Assign)


typedef enum thNodeKind thNodeKind;
enum thNodeKind {
#define N(kind) th ## kind ## Nk,
	TH_NODE_KINDS_LIST(N)
#undef N
};

typedef struct thNode thNode;
typedef struct thNode thTypeNode; // reuse

typedef thToken thNumberNode;
typedef thToken thIdentifierNode;
typedef thToken thReferenceNode;
typedef thToken thDereferenceNode;

typedef struct thArgs thArgs;
struct thArgs {
	thToken identifier;
	thTypeNode *type;
	thArgs *next;
};

typedef struct thFnNode thFnNode;
struct thFnNode {
	thArgs *args;
	thTypeNode *ret_type;
	thNode *body;
};

typedef struct thBinaryNode thBinaryNode;
struct thBinaryNode {
	thNode *left;
	thTokenKind op;
	thNode *right;
};

typedef struct thUnaryNode thUnaryNode;
struct thUnaryNode {
	thTokenKind op;
	thNode *val;
};

typedef struct thAssignNode thAssignNode;
struct thAssignNode {
	thToken name;
	thTypeNode *type;
	thNode *val;
};

struct thNode {
	thNodeKind kind;

	union {
		thBinaryNode binary;
		thUnaryNode unary;
		thNumberNode number;
		thIdentifierNode identifier;
		thReferenceNode reference;
		thDereferenceNode dereference;
		thFnNode fn;
		thAssignNode assign;
	};

	thNode *next;
};
