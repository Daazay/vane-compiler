#pragma once

#include <stdio.h>

#include "vanec/utils/defines.h"

#define MAX_STREAM_CHUNK_CAPACITY 1 * KB
#define MIN_STREAM_CHUNK_CAPACITY 8

typedef enum {
    STREAM_UNKNOWN= 0,
    STREAM_STRING,
    STREAM_FILE,
} StreamKind;

typedef struct {
    u8* data;
    u64 size;
    u64 capacity;
    bool eof;
} StreamChunk;

typedef struct {
    StreamKind kind;

    u64 size;
    u64 offset;

    union Source {
        const char* string;
        struct {
            const char* filepath;
            FILE* handle;
        } file;
    } source;
} Stream;

StreamChunk stream_chunk_create(const u64 chunk_capacity);

void stream_chunk_free(StreamChunk* chunk);

Stream stream_create(const StreamKind kind);

void stream_free(Stream* stream);

/*
  STRING_STREAM:  str => source.string
  FILE_STREAM:    str => source.file.filepath
*/
void stream_set_source(Stream* stream, const char* str);

void stream_rewind(Stream* stream);

void stream_read_next_chunk(Stream* stream, StreamChunk* chunk);