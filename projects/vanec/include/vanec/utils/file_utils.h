#pragma once

#include <stdio.h>

#include "vanec/utils/defines.h"

//u64 get_file_size(FILE* handle);

u64 get_file_size(const char* filepath);

bool is_regular_file(const char* filepath);

bool is_file_exists(const char* filepath);

bool is_dir(const char* filepath);

char* get_dirpath(const char* filepath);

char* get_filename_with_ext(const char* filepath);

char* get_filename(const char* filepath);

char* get_file_ext(const char* filepath);