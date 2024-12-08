#pragma once

#include "vanec/utils/defines.h"
#include "vanec/utils/vector.h"

typedef enum {
    COMPILER_COMMAND_UNDEFINED = 0,
    COMPILER_COMMAND_LEX_ONLY,
    COMPILER_COMMAND_PARSE_AST_ONLY,
} CompilerCommand;

typedef enum {
    COMPILER_FLAG_NONE = 0,
} CompilerFlag;

typedef struct {
    Vector files;
    const char* working_dir;
    const char* output_dir;
    u64 stream_chunk_capacity;

    u64 flags;
    CompilerCommand command;
} CompilerOptions;

void compiler_options_init(CompilerOptions* options);

bool compiler_options_parse_args(CompilerOptions* options, int argc, char** argv);

void compiler_options_free(CompilerOptions* options);