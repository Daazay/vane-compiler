#pragma once

#include "vanec/utils/defines.h"
#include "vanec/utils/stream.h"

#include "vanec/frontend/lexer/token.h"

#include "vanec/diagnostic/source_loc.h"
#include "vanec/diagnostic/diagnostic.h"

typedef struct {
    Stream* stream;
    DiagnosticEngine* diag;

    struct StreamChunk {
        void* buf;
        u64 size;
        u64 cap;
        u64 pos;

        bool last;
    } chunk;

    SourceLoc loc;
} Lexer;

Lexer* lexer_create(const u64 chunk_capacity, DiagnosticEngine* diag);

void lexer_free(Lexer* lexer);

void lexer_set_source_stream(Lexer* lexer, Stream* stream);

void lexer_rewind(Lexer* lexer);

Token lexer_parse_next_token(Lexer* lexer);