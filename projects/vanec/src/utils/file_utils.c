#include "vanec/utils/file_utils.h"

#include <assert.h>
#include <sys/stat.h>
#include <io.h>

#include "vanec/utils/string_utils.h"

#ifdef _MSC_VER
#ifndef S_ISDIR
#define S_ISDIR(mode)  (((mode) & S_IFMT) == S_IFDIR)
#endif

#if !defined(S_ISREG) && defined(S_IFMT) && defined(S_IFREG)
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif
#endif

static inline bool is_path_separator(const char c) {
#if _WIN32 
	return c == '/' || c == '\\';
#else
	return c == '/';
#endif
}

//u64 get_file_size(FILE* handle) {
//	assert(handle != NULL);
//
//    fseek(handle, 0, SEEK_END);
//    u64 size = ftell(handle);
//    rewind(handle);
//
//    return size;
//}

u64 get_file_size(const char* filepath) {
    assert(filepath != NULL);

    struct stat st;
    if (stat(filepath, &st)) {
        return 0;
    }
    return st.st_size;
}

bool is_regular_file(const char* filepath) {
    assert(filepath != NULL);

    struct stat st;
    if (stat(filepath, &st)) {
        return false;
    }
    return S_ISREG(st.st_mode);
}

bool is_file_exists(const char* filepath) {
    assert(filepath != NULL);

    struct stat st;
    if (stat(filepath, &st)) {
        return false;
    }
	return S_ISDIR(st.st_mode) || S_ISREG(st.st_mode);
}

bool is_dir(const char* filepath) {
    assert(filepath != NULL);

    struct stat st;
    if (stat(filepath, &st)) {
        return false;
    }
    return S_ISDIR(st.st_mode);
}

char* get_dirpath(const char* filepath) {
    assert(filepath != NULL);

    struct stat st;
    if (stat(filepath, &st)) {
        return NULL;
    }

    if (S_ISDIR(st.st_mode)) {
        return str_dup(filepath);
    }

    u64 len = str_len(filepath);
    if (len == 0) {
        return NULL;
    }

    u64 off = len - 1;
    while (off > 0) {
        if (is_path_separator(filepath[off])) {
            break;
        }
        --off;
    }

    if (off == 0 && !is_path_separator(filepath[off])) {
        return str_dup("./");
    }

    return str_substr(filepath, 0, off + 1);
}

char* get_filename_with_ext(const char* filepath) {
    assert(filepath != NULL);

    struct stat st;
    if (stat(filepath, &st)) {
        return NULL;
    }

    if (!S_ISREG(st.st_mode)) {
        return NULL;
    }

    u64 len = str_len(filepath);
    if (len == 0) {
        return NULL;
    }

    u64 off = len - 1;
    while (off > 0) {
        if (is_path_separator(filepath[off])) {
            break;
        }
        --off;
    }

    if (is_path_separator(filepath[off])) {
        return str_dup(filepath + off + 1);
    }

    return str_dup(filepath + off);
}

char* get_filename(const char* filepath) {
    assert(filepath != NULL);

    char* filename = get_filename_with_ext(filepath);

    if (filename == NULL) {
        return NULL;
    }

    u64 len = str_len(filename);
    u64 off = len - 1;
    do {
        if (filename[off] == '.') {
            if (off == len) {
                str_free(filename);
                return NULL;
            }
            break;
        }
    } while (--off > 0);

    char* res = str_substr(filename, 0, off);
    str_free(filename);

    return res;
}

char* get_file_ext(const char* filepath) {
    assert(filepath != NULL);

    char* filename = get_filename_with_ext(filepath);

    if (filename == NULL) {
        return NULL;
    }

    u64 len = str_len(filename);
    u64 off = len - 1;
    do {
        if (filename[off] == '.') {
            if (off == len) {
                str_free(filename);
                return NULL;
            }
            break;
        }
    } while (--off > 0);

    char* res = str_dup(filename + off);
    str_free(filename);

    return res;
}