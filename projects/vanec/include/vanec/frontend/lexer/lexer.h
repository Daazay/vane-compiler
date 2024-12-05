#pragma once

#include "vanec/utils/defines.h"
#include "vanec/utils/source_loc.h"
#include "vanec/utils/stream.h"
#include "vanec/frontend/lexer/token.h"

typedef struct {
    Stream* stream;

    StreamChunk chunk;
    u64 chunk_pos;
    SourceLoc loc;
} Lexer;

Lexer lexer_create(Stream* stream, const u64 chunk_capacity);

void lexer_free(Lexer* lexer);

void lexer_rewind(Lexer* lexer);

Token lexer_parse_next_token(Lexer* lexer);