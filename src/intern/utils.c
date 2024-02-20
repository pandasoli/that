#include "lexer/token.h"
#include "parser/node.h"
#include "utils.h"


char *token_strkind(TokenKind kind) {
	switch (kind) {
		case UnknownTk: return "Unknown";
		case EOFTk: return "EOF";

		// Symbols
		case PlusTk: return "Plus";
		case DashTk: return "Dash";
		case AsteriskTk: return "Asterisk";
		case SlashTk: return "Slash";

		case OpenParenTk: return "OpenParen";
		case CloseParenTk: return "CloseParen";

		case EqualsTk: return "Equals";

		// Literal values
		case IdentifierTk: return "Identifier";
		case IntTk: return "Int";
		case FloatTk: return "Float";
	}

	return NULL;
}

char *node_strkind(NodeKind kind) {
	switch (kind) {
		case BinaryNk: return "Binary";
		case UnaryNk: return "Unary";
		case NumberNk: return "Number";
		case VarCallNk: return "VarCall";
		case VarDeclarationNk: return "VarDeclaration";
		case AssignmentNk: return "Assignment";
	}

	return NULL;
}
