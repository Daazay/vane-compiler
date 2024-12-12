#include "vanec/vanec.h"

#include <stdio.h>

static bool set_source_file(Lexer* lexer, Stream* stream, const char* filepath) {
    printf("File - \"%s\".\n", filepath);
    if (!is_file_exists(filepath)) {
        printf("Error: File does not exists.\n");
        return false;
    }
    if (!is_regular_file(filepath)) {
        printf("Error: The file is not a regular file.\n");
        return false;
    }

    if (!stream_set_source(stream, STREAM_FILE_SOURCE, filepath)) {
        printf("Error: Something went wrong, failed to set stream source.\n");
        return false;
    }

    lexer_set_source_stream(lexer, stream);

    return true;
}

int main(int argc, char** argv) {
    CompilerOptions options = { 0 };

    compiler_options_init(&options);
    compiler_options_parse_args(&options, argc, argv);
    
    //
    DiagnosticEngine* diag = diagnostic_engine_create();
    Stream* stream = stream_create();
    Lexer* lexer = lexer_create(options.stream_chunk_capacity, diag);
    ASTParser* ast_parser = ast_parser_create(lexer, diag);
    //

    switch (options.command) {
    case COMPILER_COMMAND_LEX_ONLY: {
        for (u64 i = 0; i < options.files.items_count; ++i) {
            const char* filepath = vector_get_ref(&options.files, i);

            if (!set_source_file(lexer, stream, filepath)) {
                continue;
            }

            TokenKind token_kind = TOKEN_UNKNOWN;
            u64 tok_num = 0;
            while (token_kind != TOKEN_END_OF_FILE) {
                Token token = lexer_parse_next_token(lexer);

                char* tok_str = token_to_str(&token);
                printf("#%lld (%ld,%ld): %s\n", ++tok_num, token.loc.start.row, token.loc.start.col, tok_str);
                str_free(tok_str);

                token_kind = token.kind;
                token_free(&token);
            }

            diagnostic_engine_print_all(diag);
            diagnostic_engine_clear(diag);
        }
    } break;
    case COMPILER_COMMAND_PARSE_AST_ONLY: {
        for (u64 i = 0; i < options.files.items_count; ++i) {
            const char* filepath = vector_get_ref(&options.files, i);

            if (!set_source_file(lexer, stream, filepath)) {
                continue;
            }

            // TODO;
        }
    } break;
    };

    ast_parser_free(ast_parser);
    lexer_free(lexer);
    stream_free(stream);
    diagnostic_engine_free(diag);
    compiler_options_free(&options);
    return 0;
}