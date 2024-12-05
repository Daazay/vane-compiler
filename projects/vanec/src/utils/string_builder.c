#include "vanec/utils/string_builder.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

StringBuilder string_builder_create(void) {
    return (StringBuilder) {
        .buffer = vector_create(DEFAULT_VECTOR_CAPACITY, sizeof(char), NULL, false),
    };
}

void string_builder_free(StringBuilder* sb) {
    if (sb == NULL) {
        return;
    }

    vector_free(&sb->buffer);
}

void string_builder_append_char_left(StringBuilder* sb, const char ch) {
    assert(sb != NULL);
    assert(ch != '\0');

    vector_push_front(&sb->buffer, &ch);
}

void string_builder_append_char_right(StringBuilder* sb, const char ch) {
    assert(sb != NULL);
    assert(ch != '\0');

    vector_push_back(&sb->buffer, &ch);
}

void string_builder_append_str_left(StringBuilder* sb, const char* s) {
    assert(sb != NULL && s != NULL);

    const u64 slen = strlen(s);

    vector_insert(&sb->buffer, sb->buffer.items, s, slen);
}

void string_builder_append_str_right(StringBuilder* sb, const char* s) {
    assert(sb != NULL && s != NULL);

    const u64 slen = strlen(s);

    vector_insert(&sb->buffer, (u8*)sb->buffer.items + sb->buffer.items_count, s, slen);
}

void string_builder_append_format(StringBuilder* sb, const char* format, ...) {
    assert(sb != NULL && format != NULL);

    va_list args = { 0 };

    va_start(args, format);
    i32 count = vsnprintf(NULL, 0, format, args);
    va_end(args);

    assert(count >= 0);

    char* buffer = malloc((u64)count + 1);
    assert(buffer != NULL);

    va_start(args, format);
    count = vsnprintf(buffer, (u64)count + 1, format, args);
    va_end(args);

    assert(count >= 0);

    vector_insert(&sb->buffer, (u8*)sb->buffer.items + sb->buffer.items_count, buffer, (u64)count);

    free(buffer);
}

char* string_builder_get_str(const StringBuilder* sb) {
    assert(sb != NULL);

    char* res = malloc(sb->buffer.items_count + 1);
    assert(res != NULL);

    memcpy(res, sb->buffer.items, sb->buffer.items_count);
    res[sb->buffer.items_count] = '\0';

    return res;
}