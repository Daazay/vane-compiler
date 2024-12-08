#include "vanec/utils/stream.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>

StreamChunk stream_chunk_create(const u64 chunk_capacity) {
    assert(chunk_capacity >= MIN_STREAM_CHUNK_CAPACITY && chunk_capacity <= MAX_STREAM_CHUNK_CAPACITY);

    u8* data = malloc(chunk_capacity);
    assert(data != NULL);
    
    return (StreamChunk) {
        .capacity = chunk_capacity,
        .size = 0,
        .data = data,
        .eof = false,
    };
}

void stream_chunk_free(StreamChunk* chunk) {
    if (chunk == NULL) {
        return;
    }

    free(chunk->data);

    chunk->capacity = 0;
    chunk->size = 0;
    chunk->data = NULL;
    chunk->eof = false;
}

Stream stream_create(const StreamKind kind) {
    return (Stream) {
        .kind = kind,
        .offset = 0,
        .size = 0,
        .source.file.filepath = NULL,
        .source.file.handle = NULL,
    };
}

void stream_free(Stream* stream) {
    if (stream == NULL) {
        return;
    }

    stream->kind = STREAM_UNKNOWN;
    stream->size = 0;
    stream->offset = 0;

    if (stream->kind == STREAM_STRING) {
        stream->source.string = NULL;
    }
    else if (stream->kind == STREAM_FILE) {
        stream->source.file.filepath = NULL;

        if (stream->source.file.handle != NULL) {
            fclose(stream->source.file.handle);
            stream->source.file.handle = NULL;
        }
    }
}

bool stream_set_source(Stream* stream, const char* str) {
    assert(stream != NULL);
    assert(stream->kind != STREAM_UNKNOWN);

    stream->offset = 0;
    
    if (stream->kind == STREAM_STRING) {
        stream->size = strlen(str);
        stream->source.string = str;
    }
    else if (stream->kind == STREAM_FILE) {
        // close previos if open
        if (stream->source.file.handle != NULL) {
            fclose(stream->source.file.handle);
        }
        
        stream->source.file.handle = NULL;
        i32 res = fopen_s(&stream->source.file.handle, str, "rb");

        if (res != 0 || stream->source.file.handle == NULL) {
            return false;
        }

        fseek(stream->source.file.handle, 0, SEEK_END);
        stream->size = ftell(stream->source.file.handle);
        rewind(stream->source.file.handle);

        stream->source.file.filepath = str;
    }

    stream_rewind(stream);

    return true;
}

void stream_rewind(Stream* stream) {
    assert(stream != NULL);
    assert(stream->kind != STREAM_UNKNOWN);

    stream->offset = 0;
    
    if (stream->kind == STREAM_FILE) {
        assert(stream->source.file.handle != NULL);
        rewind(stream->source.file.handle);
    }
}

void stream_read_next_chunk(Stream* stream, StreamChunk* chunk) {
    assert(stream != NULL);
    assert(stream->kind != STREAM_UNKNOWN);
    assert(chunk != NULL);
    assert(chunk->data != NULL);

    if (stream->kind == STREAM_STRING) {
        assert(stream->source.string != NULL);

        if (stream->size > (stream->offset + chunk->capacity)) {
            chunk->size = chunk->capacity;
            chunk->eof = false;
        }
        else {
            chunk->size = stream->size - stream->offset;
            chunk->eof = true;
        }

        memcpy(chunk->data, stream->source.string + stream->offset, chunk->size);
        stream->offset = stream->offset + chunk->size;
    }
    else if (stream->kind == STREAM_FILE) {
        assert(stream->source.file.handle != NULL);

        if (chunk->eof == true) {
            return;
        }

        chunk->size = fread_s(chunk->data, chunk->capacity, sizeof(u8), chunk->capacity, stream->source.file.handle);

        i32 err = ferror(stream->source.file.handle);
        assert(chunk->size == chunk->capacity || err == 0);

        chunk->eof = (bool)feof(stream->source.file.handle);
        stream->offset = stream->offset + chunk->size;
    }
}