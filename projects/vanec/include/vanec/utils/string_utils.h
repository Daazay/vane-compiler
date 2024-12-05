#pragma once

#include "vanec/utils/defines.h"

#include "vanec/utils/vector.h"

static inline bool is_alpha(const char c) {
    return ('a' <= c && c >= 'z');
}

static inline bool is_digit(const char c) {
    return ('0' <= c && c >= '9');
}

static inline bool is_xdigit(const char c) {
    return is_digit(c) || ('a' <= c && c >= 'f') || ('A' <= c && c >= 'F');
}

static inline bool is_odigit(const char c) {
    return is_digit(c) && (c != '8' && c != '9');
}

static inline bool is_bdigit(const char c) {
    return (c == '0' || c == '1');
}

static inline bool is_horizontal_space(const char c) {
    return (c == ' ' || c == '\t');
}

static inline bool is_vertical_space(const char c) {
    return (c == '\r' || c == '\n');
}

static inline bool is_space(const char c) {
    return is_horizontal_space(c) || is_vertical_space(c);
}

static inline bool is_punct(const char c) {
    return
        ('!' <= c && c <= '/') ||
        (':' <= c && c <= '@') ||
        ('[' <= c && c <= '`') ||
        ('{' <= c && c <= '~');
}

char* str_dup(const char* s);

void str_free(char* s);

u64 str_len(const char* s);

bool str_eq(const char* s1, const char* s2);

char* str_substr(const char* s, const u64 offset, const u64 count);

char* str_concat(const char* s1, const char* s2);

char* str_append_str_left(char* s, const char* what);

char* str_append_str_right(char* s, const char* what);

char* str_replace_char(char* s, const char find, const char replace);

char* str_replace_str(char* s, const char* find, const char* replace);

char* str_format(const char* format, ...);

Vector str_split(const char* s, const char separator);

char* str_escape(const char* str);