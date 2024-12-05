#include "vanec/vanec.h"

#include <stdio.h>

int main(int argc, char** argv) {
    const char* filepath = "D:\\study\\univer\\ITMO\\system and applied software\\vane-language\\examples\\tokens.vn";

    Stream stream = stream_create(STREAM_FILE);
    stream_set_source(&stream, filepath);

    Lexer lexer = lexer_create(&stream, MIN_STREAM_CHUNK_CAPACITY);

    Token token = { 0 };
    do {
        token_free(&token);

        token = lexer_parse_next_token(&lexer);
        printf("[%lld,%ld,%ld:%lld,%ld,%ld] %s - %s \n", 
            token.loc.start.pos,
            token.loc.start.row,
            token.loc.start.col,
            token.loc.end.pos,
            token.loc.end.row,
            token.loc.end.col,
            get_token_kind_spelling(token.kind), 
            token.value
        );

    } while (token.kind != TOKEN_END_OF_FILE);


    lexer_free(&lexer);
    stream_free(&stream);

    return 0;
}