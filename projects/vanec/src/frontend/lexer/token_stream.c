#include "vanec/frontend/lexer/token_stream.h"

#include <assert.h>
#include <stdlib.h>

static TokenStreamIterator* token_stream_iterator_create(const Token token, TokenStreamIterator* prev) {
    TokenStreamIterator* it = malloc(sizeof(TokenStreamIterator));
    assert(it != NULL);

    it->next = NULL;
    it->prev = prev;
    it->token = token;

    return it;
}

static void token_stream_iterator_free(TokenStreamIterator* it) {
    while (it != NULL) {
        TokenStreamIterator* next = it->next;

        token_free(&it->token);
        free(it);

        it = next;
    }
}

TokenStream token_stream_create(Lexer* lexer) {
    assert(lexer != NULL);

    return (TokenStream) {
        .lexer = lexer,
        .head = NULL,
        .curr = NULL,
        .next = NULL,
        .count = 0,
        .done = false,
    };
}

void token_stream_free(TokenStream* ts) {
    if (ts == NULL) {
        return;
    }

    token_stream_iterator_free(ts->head);
}

void token_stream_clear (TokenStream* ts) {
    if (ts == NULL) {
        return;
    }

    token_stream_iterator_free(ts->head);

    ts->head = NULL;
    ts->curr = NULL;
    ts->next = NULL;
    ts->count = 0;
    ts->done = false;
}

static inline TokenStreamIterator* token_stream_parse_next(TokenStream* ts) {
    assert(ts != NULL);

    Token token = lexer_parse_next_token(ts->lexer);
    ++ts->count;

    TokenStreamIterator* it = token_stream_iterator_create(token, ts->curr);

    if (token.kind == TOKEN_END_OF_FILE) {
        ts->done = true;
    }

    return it;
}

const Token* token_stream_peek_next(TokenStream* ts) {
    assert(ts != NULL);

    if (ts->head == NULL) {
        ts->head = ts->next = token_stream_parse_next(ts);
        return &ts->head->token;
    }
    if (ts->curr == NULL) {
        return &ts->next->token;
    }
    if (ts->next == NULL) {
        if (ts->done) {
            return &ts->curr->token;
        }
        ts->curr->next = ts->next = token_stream_parse_next(ts);   
    }

    return&ts->next->token;
}

const Token* token_stream_consume(TokenStream* ts) {
    assert(ts != NULL);

    if (ts->next == NULL) {
        token_stream_peek_next(ts);
    }
    ts->curr = ts->next;
    ts->next = ts->curr->next;

    return &ts->curr->token;
}

void token_stream_move_back(TokenStream* ts) {
    assert(ts != NULL);

    if (ts->curr != NULL) {
        ts->next = ts->curr;
        ts->curr = ts->curr->prev;
    }
}