#pragma once

#include "vanec/utils/defines.h"
#include "vanec/utils/source_loc.h"

typedef enum {
#define TOKEN(ID, NAME, VALUE) TOKEN_##ID,
#include "vanec/frontend/lexer/token_kind.def"
} TokenKind;

typedef struct {
    TokenKind kind;
    char* value;
    SourceLoc loc;
} Token;

const char* get_token_kind_spelling(const TokenKind kind);

const char* get_token_kind_value(const TokenKind kind);

Token token_create(const TokenKind kind, char* value, const SourceLoc loc);

void token_free(Token* token);

char* token_to_str(const Token* token);