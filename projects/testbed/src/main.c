#include "vanec/vanec.h"

#include <stdio.h>

int main(int argc, char** argv) {
    CompilerOptions options = { 0 };

    compiler_options_init(&options);
    compiler_options_parse_args(&options, argc, argv);

    switch (options.command) {
    case COMPILER_COMMAND_LEX_ONLY: {
        compiler_lex(&options);
    } break;
    case COMPILER_COMMAND_PARSE_AST_ONLY: {
        compiler_parse_ast(&options);
    } break;
    };

    compiler_options_free(&options);
    return 0;
}