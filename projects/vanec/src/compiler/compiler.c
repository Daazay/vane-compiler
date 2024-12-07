#include "vanec/compiler/compiler.h"

#include "vanec/utils/stream.h"
#include "vanec/frontend/lexer/lexer.h"

void compiler_lex(CompilerOptions* options) {
    Stream fs = stream_create(STREAM_FILE);
    Lexer lexer = lexer_create(&fs, options->stream_chunk_capacity);

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

       /* TokenStream ts = token_stream_create(&lexer);

        while (!ts.done) {
            const Token* token = token_stream_get_next(&ts);

            printf("%s(%ld,%ld): %s - %s \n",
                token->loc.filepath,
                token->loc.start.row,
                token->loc.start.col,
                get_token_kind_spelling(token->kind),
                token->value
            );
        }

        token_stream_free(&ts);*/
        lexer_rewind(&lexer);
    }

    lexer_free(&lexer);
    stream_free(&fs);
}