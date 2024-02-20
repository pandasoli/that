#pragma once

#include "diags/bag.h"
#include "lexer/token.h"


typedef enum {
	BinaryNk,
	UnaryNk,
	NumberNk,
	VarCallNk,
	VarDeclarationNk,
	AssignmentNk
} NodeKind;

typedef struct Node Node;

typedef struct {
	Node *left;
	Token op;
	Node *right;
} BinaryNode;

typedef struct {
	Token op;
	Node *val;
} UnaryNode;

typedef Token NumberNode;
typedef Token VarCallNode;

typedef struct {
	Token identifier;
	Token type;
	Node *val;
} VarDeclarationNode;

typedef struct {
	Token identifier;
	Node *val;
} AssignmentNode;

struct Node {
	NodeKind kind;

	union {
		BinaryNode binary;
		UnaryNode unary;
		NumberNode number;
		VarCallNode var_call;
		VarDeclarationNode var_declaration;
		AssignmentNode assignment;
	};

	Node *next;
};

/** @internal Allocates the received node on the heap */
ERR heap_node(Node *node, DiagBag *bag, Node **out);

void free_node(Node *node);
