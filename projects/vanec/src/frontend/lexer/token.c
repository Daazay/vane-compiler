#include "vanec/frontend/lexer/token.h"

#include <assert.h>
#include <stdlib.h>

Token token_create(const TokenKind kind, char* value, const SourceLoc loc) {
	return (Token) {
		.kind = kind,
		.value = value,
		.loc = loc,
	};
}

void token_free(Token* token) {
	if (token == NULL) {
		return;
	}

	free(token->value);

	token->value = NULL;
	token->kind = TOKEN_UNKNOWN;
	token->loc.filepath = NULL;
	token->loc.start.pos = 0;
	token->loc.start.row = 0;
	token->loc.start.col = 0;
	token->loc.end.pos = 0;
	token->loc.end.row = 0;
	token->loc.end.col = 0;
}

char* token_to_str(const Token* token) {
    return NULL;
}