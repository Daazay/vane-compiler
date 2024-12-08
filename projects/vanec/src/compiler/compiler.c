#include "vanec/compiler/compiler.h"

#include <assert.h>
#include <stdio.h>

#include "vanec/utils/string_utils.h"

#include "vanec/utils/stream.h"
#include "vanec/frontend/lexer/lexer.h"

#include "vanec/frontend/ast/ast_parser.h"
#include "vanec/frontend/ast/ast_node_utils.h"

void compiler_lex(CompilerOptions* options) {
    DiagnosticEngine diag = diagnostic_engine_create();
    Stream fs = stream_create(STREAM_FILE);
    Lexer lexer = lexer_create(&fs, options->stream_chunk_capacity, &diag);

    for (u64 i = 0; i < options->files.items_count; ++i) {
        const char* filepath = vector_get_ref(&options->files, i);
        stream_set_source(&fs, filepath);

        TokenKind kind = 0;
        do {
            Token token = lexer_parse_next_token(&lexer);
            kind = token.kind;

            printf("%s(%ld,%ld): %s - %s \n",
                token.loc.filepath,
                token.loc.start.row,
                token.loc.start.col,
                get_token_kind_spelling(token.kind),
                token.value
            );

            token_free(&token);
        } while (kind != TOKEN_END_OF_FILE);
        lexer_rewind(&lexer);
    }

    diagnostic_engine_print_all(&diag);

    lexer_free(&lexer);
    stream_free(&fs);
    diagnostic_engine_free(&diag);
}

bool write_ast_dot_file(const char* filepath, const Vector* functions) {
    assert(filepath != NULL);

    FILE* file = NULL;
    i32 status = fopen_s(&file, filepath, "wb");
    if (status != 0 || file == NULL) {
        return false;
    }

    write_dot_header(file);
    for (u64 i = 0; i < functions->items_count; ++i) {
        const ASTNode* node = vector_get_ref(functions, i);
        write_ast_node_to_dot_file(file, NULL, node);
    }
    putc('}', file);

    fclose(file);

    return true;
}

void compiler_parse_ast(CompilerOptions* options) {
    DiagnosticEngine diag = diagnostic_engine_create();
    Stream fs = stream_create(STREAM_FILE);
    Lexer lexer = lexer_create(&fs, options->stream_chunk_capacity, &diag);
    ASTParser parser = ast_parser_create(&lexer, &diag);

    Vector functions = vector_create(DEFAULT_VECTOR_CAPACITY, sizeof(ASTNode*), &ast_node_free, true);

    for (u64 i = 0; i < options->files.items_count; ++i) {
        const char* filepath = vector_get_ref(&options->files, i);
        stream_set_source(&fs, filepath);

        while (!is_ast_parser_done(&parser)) {
            ASTNode* funcdef = ast_parser_parse_ast_funcdef_node(&parser);

            if (funcdef != NULL) {
                vector_push_back(&functions, &funcdef);
            }
            else {
                ast_parser_skip_to_the_end_of_a_file(&parser);
            }
        }

        char* output_filepath = str_concat(filepath, ".dot");
        write_ast_dot_file(output_filepath, &functions);

        str_free(output_filepath);
        vector_free(&functions);
        lexer_rewind(&lexer);
        diagnostic_engine_clear(&diag);
    }

    diagnostic_engine_print_all(&diag);

    ast_parser_free(&parser);
    lexer_free(&lexer);
    stream_free(&fs);
    diagnostic_engine_free(&diag);
}