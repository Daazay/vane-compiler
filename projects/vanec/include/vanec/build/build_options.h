#pragma once

#include "vanec/utils/defines.h"
#include "vanec/utils/vector.h"

typedef enum {
    BUILD_COMMAND_MISSING = 0,
    BUILD_COMMAND_LEX,
    BUILD_COMMAND_PARSE_AST,
} BuildCommand;

typedef struct {
    Vector files;
    BuildCommand command;
    // options
    bool print_tokens;
} BuildOptions;

void build_options_free(BuildOptions* options);

void parse_arguments(BuildOptions* options, int argc, char** argv);