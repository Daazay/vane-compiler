#include "vanec/utils/stream.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>

Stream* stream_create() {
    Stream* stream = malloc(sizeof(Stream));
    assert(stream != NULL);

    *stream = (Stream) {
        .kind = STREAM_UNKNOWN_SOURCE,
        .offset = 0,
        .len = 0,
        .done = false,
        .source.file.filepath = NULL,
        .source.file.handle = NULL,
    };

    return stream;
}

void stream_free(Stream* stream) {
    if (stream == NULL) {
        return;
    }
    
    if (stream->kind == STREAM_FILE_SOURCE) {
        if (stream->source.file.handle != NULL) {
            fclose(stream->source.file.handle);
        }
    }

    free(stream);
}

static inline void stream_set_string_source(Stream* stream, const char* string) {
    assert(stream != NULL && string != NULL);

    stream->offset = 0;
    stream->kind = STREAM_STRING_SOURCE;
    stream->done = false;

    stream->len = strlen(string);
    stream->source.string = string;
}

static inline bool stream_set_file_source(Stream* stream, const char* filepath) {
    assert(stream != NULL && filepath != NULL);

    stream->offset = 0;
    stream->kind = STREAM_FILE_SOURCE;
    stream->done = false;

    stream->source.file.handle = NULL;
    i32 res = fopen_s(&stream->source.file.handle, filepath, "rb");

    if (res != 0 || stream->source.file.handle == NULL) {
        return false;
    }

    fseek(stream->source.file.handle, 0, SEEK_END);
    stream->len = ftell(stream->source.file.handle);
    rewind(stream->source.file.handle);

    stream->source.file.filepath = filepath;

    return true;
}

bool stream_set_source(Stream* stream, const StreamSourceKind kind, const char* filepath) {
    assert(stream != NULL && kind != STREAM_UNKNOWN_SOURCE && filepath != NULL);
    
    if (kind == STREAM_STRING_SOURCE) {
        stream_set_string_source(stream, filepath);
        return true;
    }
    else if (kind == STREAM_FILE_SOURCE) {
        return stream_set_file_source(stream, filepath);
    }

    return false;
}

void stream_close(Stream* stream) {
    assert(stream != NULL);

    stream->done = false;
    stream->len = 0;
    stream->offset = 0;

    if (stream->kind == STREAM_STRING_SOURCE) {
        stream->source.string = NULL;
    }
    else if (stream->kind == STREAM_FILE_SOURCE) {
        stream->source.file.filepath = NULL;
        if (stream->source.file.handle != NULL) {
            fclose(stream->source.file.handle);
        }
    }

    stream->kind = STREAM_UNKNOWN_SOURCE;
}

void stream_rewind(Stream* stream) {
    assert(stream != NULL);
    assert(stream->kind != STREAM_UNKNOWN_SOURCE);

    stream->offset = 0;
    stream->done = false;

    if (stream->kind == STREAM_FILE_SOURCE) {
        assert(stream->source.file.handle != NULL);
        rewind(stream->source.file.handle);
    }
}

bool stream_read_next_chunk(Stream* stream, void* buf, const u64 cap, u64* len) {
    assert(stream != NULL && buf != NULL && cap != 0 && len != NULL);
    assert(stream->kind != STREAM_UNKNOWN_SOURCE);

    if (stream->done) {
        return false;
    }

    if (stream->kind == STREAM_STRING_SOURCE) {
        assert(stream->source.string != NULL);

        if (stream->len > (stream->offset + cap)) {
            *len = cap;
        }
        else {
            *len = stream->len - stream->offset;
            stream->done = true;
        }

        memcpy(buf, stream->source.string + stream->offset, *len);
        stream->offset = stream->offset + *len;

        return stream->done;
    }
    else if (stream->kind == STREAM_FILE_SOURCE) {
        assert(stream->source.file.handle != NULL);

        *len = fread_s(buf, cap, sizeof(u8), cap, stream->source.file.handle);

        i32 err = ferror(stream->source.file.handle);
        assert(*len == cap || err == 0);

        stream->done = (bool)feof(stream->source.file.handle);
        stream->offset = stream->offset + *len;

        return stream->done;
    }

    return false;
}