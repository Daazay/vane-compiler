#pragma once

#include "vanec/utils/vector.h"

typedef struct {
    Vector buffer;
} StringBuilder;

StringBuilder string_builder_create();

void string_builder_free(StringBuilder* sb);

void string_builder_append_char_left(StringBuilder* sb, const char ch);

void string_builder_append_char_right(StringBuilder* sb, const char ch);

void string_builder_append_str_left(StringBuilder* sb, const char* s);

void string_builder_append_str_right(StringBuilder* sb, const char* s);

void string_builder_append_format(StringBuilder* sb, const char* format, ...);

char* string_builder_get_str(const StringBuilder* sb);