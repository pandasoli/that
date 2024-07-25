#pragma once

#include <thatlang/lexer/token.h>
#include <thatlang/parser/node.h>


thERR node_binary_create(thNode *left, thToken op, thNode *right, thNode **out);
thERR node_unary_create(thToken op, thNode *val, thNode **out);
thERR node_number_create(thToken val, thNode **out);
