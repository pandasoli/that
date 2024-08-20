#pragma once

#include <thatlang/lexer/token.h>
#include <thatlang/parser/node.h>


thERR node_type_create(thTypeNode *type, thTypeNode **out);

thERR node_identifier_create(thToken identifier, thNode **out);
thERR node_fn_create(thTypeNode *ret_type, thNode *body, thNode **out);
thERR node_assign_create(thToken name, thTypeNode *type, thNode *val, thNode **out);
thERR node_binary_create(thNode *left, thToken op, thNode *right, thNode **out);
thERR node_unary_create(thToken op, thNode *val, thNode **out);
thERR node_number_create(thToken val, thNode **out);
