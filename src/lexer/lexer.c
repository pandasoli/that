#include "lexer/lexer.h"
#include "lexer/token.h"
#include "globl.h"

#define NEXT self->pos++
#define CURRENT self->text[self->pos]

#define IS_ID(a) ('a' <= a && a <= 'z') || ('A' <= a && a <= 'Z') || a == '_'
#define IS_DEC(a) ('0' <= a && a <= '9')


static Token lex(Lexer *self) {
	while (CURRENT == ' ') NEXT;

	TokenKind kind = UnknownTk;
	size_t pos = self->pos;
	char *literal = self->text + pos;

	switch (CURRENT) {
		case   0: NEXT; kind = EOFTk; break;

		case '+': NEXT; kind = PlusTk; break;
		case '-': NEXT; kind = DashTk; break;
		case '*': NEXT; kind = AsteriskTk; break;
		case '/': NEXT; kind = SlashTk; break;
		case '(': NEXT; kind = OpenParenTk; break;
		case ')': NEXT; kind = CloseParenTk; break;
		case '=': NEXT; kind = EqualsTk; break;

		default:
			/* Numbers support:
			 * int		123
			 * float	123.456
			 */
			if (IS_DEC(CURRENT)) {
				kind = IntTk;

				// Get the integer part
				while (IS_DEC(CURRENT))
					NEXT;

				// Identify first dot within int
				if (CURRENT == '.') {
					kind = FloatTk;
					NEXT;

					while (IS_DEC(CURRENT))
						NEXT;
				}
			}

			// [a-zA-Z_][a-zA-Z_0-1]*
			else if (IS_ID(CURRENT)) {
		    kind = IdentifierTk;

	    	while (IS_ID(CURRENT) || IS_DEC(CURRENT))
  	      NEXT;
			}

			// Just skip Unknown
			else NEXT;
	}

	return new_token(kind, literal, pos, self->pos - pos);
}

ERR lexer_init(char *text, Lexer *lexer) {
	if (text == NULL || lexer == NULL)
		return 1;

	lexer->text = text;
	lexer->pos = 0;

	lexer->lex = &lex;

	return 0;
}
