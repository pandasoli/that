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
		case SlashTk: return "Slash";

		case CommaTk: return "Comma";
		case OpenBraceTk: return "OpenBrace";
		case CloseBraceTk: return "CloseBrace";
		case OpenParenTk: return "OpenParen";
		case CloseParenTk: return "CloseParen";

		case LessThanTk: return "LessThan";
		case GreaterThanTk: return "GreaterThan";
		case EqualsToTk: return "EqualsTo";
		case DiffTk: return "Diff";

		case AmpersandTk: return "Ampersand";
		case AsteriskTk: return "Asterisk";
		case PipeTk: return "Pipe";
		case EqualsTk: return "Equals";

		// Literals
		case IdentifierTk: return "Identifier";
		case IntTk: return "Int";
		case FloatTk: return "Float";

		// Keywords
		case ImportTk: return "Import";
		case PubTk: return "Pub";
		case LocalTk: return "Local";
		case TypeTk: return "Type";
		case AsTk: return "As";
		case NotTk: return "Not";
		case AndTk: return "And";
		case OrTk: return "Or";
		case FnTk: return "Fn";
		case ReturnTk: return "Return";
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
