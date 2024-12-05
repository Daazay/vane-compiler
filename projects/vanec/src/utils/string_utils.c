#include "vanec/utils/string_utils.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

char* str_dup(const char* s) {
    assert(s != NULL);
    return _strdup(s);
}

void str_free(char* s) {
    free(s);
}

u64 str_len(const char* s) {
    assert(s != NULL);
    return strlen(s);
}

bool str_eq(const char* s1, const char* s2) {
    assert(s1 != NULL && s2 != NULL);
    return strcmp(s1, s2) == 0;
}

char* str_substr(const char* s, const u64 offset, const u64 count) {
    assert(s != NULL);

    const u64 slen = strlen(s);

    if (slen <= offset) {
        return str_dup("");
    }

    const u64 reslen = (slen > (offset + count))
        ? count
        : slen - offset;

    char* res = malloc(reslen + 1);
    assert(res != NULL);

    memcpy(res, s + offset, reslen);
    res[reslen] = '\0';

    return res;
}

char* str_concat(const char* s1, const char* s2) {
    assert(s1 != NULL && s2 != NULL);

    const u64 len1 = strlen(s1);
    const u64 len2 = strlen(s2);

    char* res = malloc(len1 + len2 + 1);
    assert(res != NULL);

    memcpy(res, s1, len1);
    memcpy(res + len1, s2, len2);
    res[len1 + len2] = '\0';

    return res;
}

char* str_append_str_left(char* s, const char* what) {
    assert(s != NULL && what != NULL);

    char* res = str_concat(s, what);
    free(s);
    return res;
}

char* str_append_str_right(char* s, const char* what) {
    assert(s != NULL && what != NULL);

    char* res = str_concat(what, s);
    free(s);
    return res;
}

char* str_replace_char(char* s, const char find, const char replace) {
    assert(s != NULL);

    char* ptr = strchr(s, find);
    while (ptr != NULL) {
        *ptr = replace;
        ptr = strchr(ptr, find);
    }

    return s;
}

char* str_replace_str(char* s, const char* find, const char* replace) {
    assert(s != NULL && find != NULL && replace != NULL);

    const u64 s_len = strlen(s);
    const u64 find_len = strlen(find);
    const u64 rep_len = strlen(replace);

    assert(find_len != 0);

    u64 rep_count = 0;
    char* tmp = NULL;
    char* res = NULL;
    const char* s_ptr = s;

    while ((tmp = strstr(s_ptr, find)) != NULL) {
        ++rep_count;
        s_ptr = tmp + find_len;
    }

    if (rep_count == 0) {
        return s;
    }

    const u64 res_len = s_len + ((i64)rep_len - (i64)find_len) * rep_count;

    res = tmp = malloc(res_len + 1);
    assert(tmp != NULL);

    res[res_len] = '\0';

    s_ptr = s;
    const char* ins_ptr = NULL;

    u64 orig_len = 0;
    while (rep_count--) {
        ins_ptr = strstr(s_ptr, find);
        orig_len = ins_ptr - s_ptr;
        memcpy(tmp, s_ptr, orig_len);
        tmp += orig_len;
        memcpy(tmp, replace, rep_len);
        tmp += rep_len;
        s_ptr = ins_ptr + find_len;
    }

    // append rest
    orig_len = strlen(s_ptr);
    memcpy(tmp, s_ptr, orig_len);

    free(s);

    return res;
}

char* str_format(const char* format, ...) {
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

    return buffer;
}

Vector str_split(const char* s, const char separator) {
    assert(s != NULL);
    assert(separator != '\0');

    Vector strings = vector_create(DEFAULT_VECTOR_CAPACITY, sizeof(char*), &free, true);

    const char* str = s;
    const char* ptr = strchr(str, separator);

    while (ptr != NULL) {
        const u64 len = ptr - s;
        
        char* res = malloc(len + 1);
        assert(res != NULL);

        memcpy(res, ptr, len);
        res[len] = '\0';

        vector_push_back(&strings, &res);

        str = ptr;
        ptr = strchr(str, separator);
    }

    return strings;
}

char* str_escape(const char* str) {
    assert(str != NULL);

    return str_dup(str);
}