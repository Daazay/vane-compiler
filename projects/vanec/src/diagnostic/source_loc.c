#include "vanec/diagnostic/source_loc.h"

SourcePos source_pos_create(const u64 pos, const u32 row, const u32 col) {
    return (SourcePos) {
        .pos = pos,
        .row = row,
        .col = col,
    };
}

SourceLoc source_loc_create(const char* filepath, const SourcePos start, const SourcePos end) {
    return (SourceLoc) {
        .filepath = filepath,
        .start = start,
        .end = end,
    };
}