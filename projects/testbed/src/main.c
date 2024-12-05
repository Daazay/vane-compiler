#include "vanec/vanec.h"

#include <stdio.h>

#define DEFER_RETURN(res, msg, ...) \
do {                                \
    result = res;                   \
    printf(msg, ##__VA_ARGS__);     \
    goto defer_return;              \
} while (false)

int main(int argc, char** argv) {
    i32 result = 0;

    BuildOptions options = { 0 };
    parse_arguments(&options, argc, argv);

    switch (options.command) {
    case BUILD_COMMAND_LEX: {
        if (options.files.items_count == 0) {
            DEFER_RETURN(-1, "Error: File for \"lex\" command is not provided.");
        }

        const char* file = vector_get_ref(&options.files, 0);
        
        Stream stream = stream_create(STREAM_FILE);
        
        if (!stream_set_source(&stream, file)) {
            DEFER_RETURN(-1, "Error: Failed to open a file - \"%s\".", file);
        }

        Lexer lexer = lexer_create(&stream, MIN_STREAM_CHUNK_CAPACITY);

        TokenStream ts = token_stream_create(&lexer);

        while (!ts.done) {
            const Token* token = token_stream_get_next(&ts);

            if (options.print_tokens) {
                printf("%s(%ld,%ld): %s - %s \n",
                    token->loc.filepath,
                    token->loc.start.row,
                    token->loc.start.col,
                    get_token_kind_spelling(token->kind),
                    token->value
                );
            }
        }

        token_stream_free(&ts);
        lexer_free(&lexer);
        stream_free(&stream);
    } break;
    case BUILD_COMMAND_PARSE_AST: {
        return -1;
    } break;
    default: {
        return -1;
    } break;
    };

defer_return:
    build_options_free(&options);
    return result;
}