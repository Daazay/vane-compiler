#pragma once

#include <stdio.h>

#include "vanec/utils/defines.h"

typedef enum {
    STREAM_UNKNOWN_SOURCE = 0,
    STREAM_STRING_SOURCE  = 1,
    STREAM_FILE_SOURCE    = 2,
} StreamSourceKind;

typedef struct {
    StreamSourceKind kind;

    u64 len;
    u64 offset;
    bool done;

    union Source {
        const char* string;
        struct {
            const char* filepath;
            FILE* handle;
        } file;
    } source;
} Stream;

Stream* stream_create();

void stream_free(Stream* stream);

bool stream_set_source(Stream* stream, const StreamSourceKind kind, const char* filepath);

void stream_close(Stream* stream);

void stream_rewind(Stream* stream);

bool stream_read_next_chunk(Stream* stream, void* buf, const u64 cap, u64* len);