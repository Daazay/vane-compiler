#pragma once

#include "vanec/utils/defines.h"
#include "vanec/utils/source_loc.h"
#include "vanec/frontend/lexer/token_kind.h"

typedef struct {
    TokenKind kind;
    char* value;
    SourceLoc loc;
} Token;

Token token_create(const TokenKind kind, char* value, const SourceLoc loc);

void token_free(Token* token);

char* token_to_str(const Token* token);