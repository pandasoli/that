#include <thatlang/lexer/lexer.h>
#include <thatlang/lexer/token.h>
#include <thatlang/globl.h>

#include <string.h>

#define NEXT self->pos++
#define CURRENT self->text[self->pos]

#define IS_ID(a) ('a' <= a && a <= 'z') || ('A' <= a && a <= 'Z') || a == '_'
#define IS_DEC(a) ('0' <= a && a <= '9')


static thKeywordInfo keyword_infos[] = {
#define T(kind)
#define KW(kind, image) {kind ## Kw, image},
	TOKEN_KINDS_LIST(T, KW)
#undef T
#undef KW
	{0, NULL}
};

static thToken lex(thLexer *self) {
	while (
		CURRENT == ' ' ||
		CURRENT == '\t' ||
		CURRENT == '\n'
	) NEXT;

	thTokenKind kind = UnknownTk;
	size_t pos = self->pos;
	char *literal = self->text + pos;

	switch (CURRENT) {
		case   0: NEXT; kind = EOFTk; break;

		// Symbols
		case '+': NEXT; kind = PlusTk; break;
		case '-': {
			NEXT;

			// Comment
			if (CURRENT == '-') {
				while (CURRENT != '\n' && CURRENT != 0)
					NEXT;

				return lex(self);
			}
			else
				kind = DashTk;
		} break;
		case '/': NEXT; kind = SlashTk; break;

		case ',': NEXT; kind = CommaTk; break;
		case '{': NEXT; kind = OpenBraceTk; break;
		case '}': NEXT; kind = CloseBraceTk; break;
		case '(': NEXT; kind = OpenParenTk; break;
		case ')': NEXT; kind = CloseParenTk; break;

		case '<': NEXT; kind = LessThanTk; break;
		case '>': NEXT; kind = GreaterThanTk; break;

		case '=': {
			kind = EqualsTk;
			NEXT;

			if (CURRENT == '=') {
				kind = EqualsToTk;
				NEXT;
			}
		} break;
		case '!': {
			NEXT;

			if (CURRENT == '=') {
				kind = DiffTk;
				NEXT;
			}
		} break;

		case '&': NEXT; kind = AmpersandTk; break;
		case '*': NEXT; kind = AsteriskTk; break;
		case '|': NEXT; kind = PipeTk; break;

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

				// Keywords
				size_t len = self->pos - pos;

				for (size_t i = 0; keyword_infos[i].kind != 0 && kind == IdentifierTk; i++)
					if (strncmp(keyword_infos[i].image, literal, len) == 0)
						kind = keyword_infos[i].kind;
			}

			// Just skip Unknown
			else NEXT;
	}

	return th_token_create(kind, literal, pos, self->pos - pos);
}

thERR th_lexer_init(char *text, thLexer *lexer) {
	if (text == NULL || lexer == NULL)
		return 1;

	lexer->text = text;
	lexer->pos = 0;

	lexer->lex = &lex;

	return 0;
}
