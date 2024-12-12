#pragma once

#include "vanec/utils/defines.h"

#define DEFAULT_ROW_INDEX 1
#define DEFAULT_COL_INDEX 1

typedef struct {
    u64 pos;
    u32 row;
    u32 col;
} SourcePos;

typedef struct {
    const char* filepath;
    SourcePos start;
    SourcePos end;
} SourceLoc;

SourcePos source_pos_create(const u64 pos, const u32 row, const u32 col);

SourceLoc source_loc_create(const char* filepath, const SourcePos start, const SourcePos end);