#pragma once

#include "vanec/utils/defines.h"
#include "vanec/frontend/lexer/token.h"
#include "vanec/frontend/lexer/lexer.h"

typedef struct TokenStreamIterator TokenStreamIterator;

struct TokenStreamIterator {
    Token token;
    TokenStreamIterator* prev;
    TokenStreamIterator* next;
};

typedef struct {
    Lexer* lexer;

    TokenStreamIterator* head;
    TokenStreamIterator* curr;
    TokenStreamIterator* next;

    u64 count;
    bool done;
} TokenStream;

TokenStream token_stream_create(Lexer* lexer);

void token_stream_free(TokenStream* ts);

void token_stream_clear(TokenStream* ts);

const Token* token_stream_peek_next(TokenStream* ts);

const Token* token_stream_consume(TokenStream* ts);

void token_stream_move_back(TokenStream* ts);