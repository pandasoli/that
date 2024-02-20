#pragma once

#include "lexer/token.h"
#include "parser/node.h"


char *token_strkind(TokenKind kind);
char *node_strkind(NodeKind kind);
