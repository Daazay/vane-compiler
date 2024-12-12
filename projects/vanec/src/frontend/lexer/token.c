#include "vanec/frontend/lexer/token.h"

#include <assert.h>
#include <stdlib.h>

#include "vanec/utils/string_utils.h"

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
    if (token == NULL) {
        return NULL;
    }
    if (is_token_kind_a_punctuator(token->kind)) {
        return str_format("punct(\'%s\')", get_token_kind_value(token->kind));
    }
    else if (is_token_kind_a_literal(token->kind)) {
        if (token->kind == TOKEN_STRING_LITERAL) {
            return str_format("literal(\"%s\")", token->value);
        }
        else if (token->kind == TOKEN_CHAR_LITERAL) {
            return str_format("literal(\'%s\')", token->value);
        }
        return str_format("literal(%s)", token->value);
    }
    else if (is_token_kind_a_keyword(token->kind)) {
        return str_format("keyword(%s)", get_token_kind_value(token->kind));
    }
    else if (token->kind == TOKEN_IDENTIFIER) {
        return str_format("identifier(%s)", token->value);
    }
    else if (token->kind == TOKEN_UNKNOWN) {
        return str_format("unknown(%s)", get_token_kind_value(token->kind));
    }
    return str_format("misc(%s)", get_token_kind_spelling(token->kind));
}