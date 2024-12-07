#include "vanec/vanec.h"

#include <stdio.h>

int main(int argc, char** argv) {
    CompilerOptions options = { 0 };

    u32 args_count = 3;
    char* args[] = {
        "",
        "lex",
        "D:/study/univer/ITMO/system and applied software/vane-language/examples/fizz_buzz.vn"
    };

    compiler_options_init(&options);

    if (!compiler_options_parse_args(&options, args_count, args)) {
        compiler_options_free(&options);
        return -1;
    }

    switch (options.command) {
    case COMPILER_COMMAND_LEX_ONLY: {
        compiler_lex(&options);
    } break;
    };

    compiler_options_free(&options);
    return 0;
}