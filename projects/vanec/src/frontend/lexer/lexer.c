#include "vanec/frontend/lexer/lexer.h"

#include <assert.h>
#include <stdlib.h>

#include "vanec/utils/string_utils.h"
#include "vanec/utils/string_builder.h"

Lexer* lexer_create(const u64 chunk_capacity, DiagnosticEngine* diag) {
    assert(chunk_capacity >= MIN_STREAM_CHUNK_CAPACITY && chunk_capacity <= MAX_STREAM_CHUNK_CAPACITY);

    Lexer* lexer = malloc(sizeof(Lexer));
    assert(lexer != NULL);

    void* buf = malloc(chunk_capacity);
    assert(buf != NULL);

    *lexer = (Lexer) {
        .stream = NULL,
        .diag = diag,
        .chunk.buf = buf,
        .chunk.cap = chunk_capacity,
        .chunk.pos = 0,
        .chunk.size = 0,
        .chunk.last = false,
        .loc = { 0 },
    };

    return lexer;
}

void lexer_free(Lexer* lexer) {
    if (lexer == NULL) {
        return;
    }

    free(lexer->chunk.buf);
    free(lexer);
}

void lexer_set_source_stream(Lexer* lexer, Stream* stream) {
    assert(lexer != NULL && stream != NULL && stream->kind != STREAM_UNKNOWN_SOURCE);

    lexer->stream = stream;

    lexer->chunk.pos = 0;
    lexer->chunk.size = 0;
    lexer->chunk.last = false;

    lexer->loc = (SourceLoc){
        .filepath = (stream->kind == STREAM_STRING_SOURCE)
            ? stream->source.string
            : stream->source.file.filepath,
        .start.pos = 0,
        .start.row = DEFAULT_ROW_INDEX,
        .start.col = DEFAULT_COL_INDEX,
        .end.pos = 0,
        .end.row = DEFAULT_ROW_INDEX,
        .end.col = DEFAULT_COL_INDEX,
    };
}

void lexer_rewind(Lexer* lexer) {
    if (lexer == NULL) {
        return;
    }

    stream_rewind(lexer->stream);

    lexer->chunk.pos = 0;
    lexer->chunk.size = 0;

    lexer->loc.filepath = lexer->stream->source.file.filepath;

    lexer->loc.start.pos = 0;
    lexer->loc.start.row = DEFAULT_ROW_INDEX;
    lexer->loc.start.col = DEFAULT_COL_INDEX;

    lexer->loc.end.pos = 0;
    lexer->loc.end.row = DEFAULT_ROW_INDEX;
    lexer->loc.end.col = DEFAULT_COL_INDEX;
}

static inline char get_curr_char_from_stream(Lexer* lexer) {
    assert(lexer != NULL);

    if (lexer->chunk.pos >= lexer->chunk.size) {
        if (lexer->chunk.last) {
            return '\0';
        }
        lexer->chunk.last = stream_read_next_chunk(lexer->stream, lexer->chunk.buf, lexer->chunk.cap, &lexer->chunk.size);
        lexer->chunk.pos = 0;
    }

    if (lexer->chunk.size == 0) {
        return '\0';
    }

    const u8* data = (u8*)lexer->chunk.buf;
    return data[lexer->chunk.pos];
}

static char consume_char(Lexer* lexer) {
    assert(lexer != NULL);

    char ch = get_curr_char_from_stream(lexer);

    if (ch != '\0') {
        ++lexer->chunk.pos;
        ++lexer->loc.end.col;
        ++lexer->loc.end.pos;
    }

    return ch;
}

static void sync_loc(Lexer* lexer) {
    assert(lexer != NULL);

    lexer->loc.start.pos = lexer->loc.end.pos;
    lexer->loc.start.row = lexer->loc.end.row;
    lexer->loc.start.col = lexer->loc.end.col;
}

static void skip_whitespaces(Lexer* lexer) {
    assert(lexer != NULL);

    bool has_r = false;
    
    char ch = '\0';
    while ((ch = get_curr_char_from_stream(lexer)) != '\0') {
        if (ch == '\r') {
            ++lexer->chunk.pos;
            ++lexer->loc.end.pos;
            ++lexer->loc.end.row;
            lexer->loc.end.col = DEFAULT_COL_INDEX;

            has_r = true;
            continue;
        }
        else if (ch == '\n') {
            ++lexer->chunk.pos;
            ++lexer->loc.end.pos;

            if (!has_r) {
                ++lexer->loc.end.row;
                lexer->loc.end.col = DEFAULT_COL_INDEX;
            }

            has_r = false;
            continue;
        }
        else if (is_horizontal_space(ch)) {
            consume_char(lexer);

            has_r = false;
            continue;
        }
        break;
    }

    sync_loc(lexer);
}

static Token lexer_parse_c1(Lexer* lexer, const char c, const TokenKind kind1, const TokenKind kind2) {
    assert(lexer != NULL);
    assert(c != '\0');

    char tmp_c = get_curr_char_from_stream(lexer);
    if (tmp_c == c) {
        consume_char(lexer);
        return token_create(kind2, NULL, lexer->loc);
    }
    return token_create(kind1, NULL, lexer->loc);
}

static Token lexer_parse_c2(Lexer* lexer, const char c1, const char c2, const TokenKind kind1, const TokenKind kind21, const TokenKind kind22) {
    assert(lexer != NULL);
    assert(c1 != '\0' && c2 != '\0');

    char tmp_c = get_curr_char_from_stream(lexer);
    if (tmp_c == c1) {
        consume_char(lexer);
        return token_create(kind21, NULL, lexer->loc);
    }
    else if (tmp_c == c2) {
        consume_char(lexer);
        return token_create(kind22, NULL, lexer->loc);
    }
    return token_create(kind1, NULL, lexer->loc);
}

static Token lexer_parse_c2c1(Lexer* lexer, const char c21, const char c22, const char c3, const TokenKind kind1, const TokenKind kind21, const TokenKind kind22, const TokenKind kind3) {
    assert(lexer != NULL);
    assert(c21 != '\0' && c22 != '\0' && c3 != '\0');

    char tmp_c = get_curr_char_from_stream(lexer);
    if (tmp_c == c21) {
        consume_char(lexer);
        return token_create(kind21, NULL, lexer->loc);
    }
    else if (tmp_c == c22) {
        consume_char(lexer);
        tmp_c = get_curr_char_from_stream(lexer);
        if (tmp_c == c3) {
            consume_char(lexer);
            return token_create(kind3, NULL, lexer->loc);
        }
        return token_create(kind22, NULL, lexer->loc);
    }
    return token_create(kind1, NULL, lexer->loc);
}

static void skip_single_line_comment(Lexer* lexer) {
    assert(lexer != NULL);

    bool has_r = false;
    bool has_backslash = false;

    char ch = '\0';
    while ((ch = get_curr_char_from_stream(lexer)) != '\0') {
        if (ch == '\\') {
            consume_char(lexer);

            has_r = false;
            has_backslash = false;
            continue;
        }
        else if (ch == '\r') {
            if (!has_backslash) {
                break;
            }

            ++lexer->chunk.pos;
            ++lexer->loc.end.pos;
            ++lexer->loc.end.row;
            lexer->loc.end.col = DEFAULT_COL_INDEX;

            has_r = true;
            has_backslash = false;
            continue;
        }
        else if (ch == '\n') {
            if (!has_backslash) {
                break;
            }

            ++lexer->chunk.pos;
            ++lexer->loc.end.pos;

            if (!has_r) {
                ++lexer->loc.end.row;
                lexer->loc.end.col = DEFAULT_COL_INDEX;
            }

            has_r = false;
            has_backslash = false;
            continue;
        }
        consume_char(lexer);

        has_r = false;
        has_backslash = false;
    }
}

static bool skip_multi_line_comment(Lexer* lexer) {
    assert(lexer != NULL);

    bool has_r = false;
    bool has_star = false;
    bool is_good = false;

    char ch = '\0';
    while ((ch = get_curr_char_from_stream(lexer)) != '\0') {
        if (ch == '*') {
            consume_char(lexer);

            has_r = false;
            has_star = true;
            continue;
        }
        else if (ch == '\r') {
            ++lexer->chunk.pos;
            ++lexer->loc.end.pos;
            ++lexer->loc.end.row;
            lexer->loc.end.col = DEFAULT_COL_INDEX;

            has_r = true;
            has_star = false;
            continue;
        }
        else if (ch == '\n') {
            ++lexer->chunk.pos;
            ++lexer->loc.end.pos;

            if (!has_r) {
                ++lexer->loc.end.row;
                lexer->loc.end.col = DEFAULT_COL_INDEX;
            }

            has_r = true;
            has_star = false;
            continue;
        }
        else if (ch == '/' && has_star) {
            consume_char(lexer);

            is_good = true;
            break;
        }
        consume_char(lexer);

        has_r = false;
        has_star = false;
    }

    if (!is_good && lexer->diag != NULL) {
        diagnostic_engine_report(lexer->diag, ERR_UNTERMINATED_COMMENT_BLOCK, lexer->loc);
    }

    return is_good;
}

Token lexer_parse_string_literal(Lexer* lexer) {
    assert(lexer != NULL);

    bool has_r = false;
    bool has_backslash = false;
    bool is_good = false;

    StringBuilder sb = string_builder_create();

    char ch = '\0';
    while ((ch = get_curr_char_from_stream(lexer)) != '\0') {
        if (ch == '\\' && !has_backslash) {
            string_builder_append_char_right(&sb, ch);
            consume_char(lexer);

            has_r = false;
            has_backslash = true;
            continue;
        }
        else if (ch == '\"' && !has_backslash) {
            consume_char(lexer);
            
            is_good = true;
            break;
        }
        else if (ch == '\r') {
            if (!has_backslash) {
                break;
            }

            string_builder_append_char_right(&sb, ch);
            
            ++lexer->chunk.pos;
            ++lexer->loc.end.pos;
            ++lexer->loc.end.row;
            lexer->loc.end.col = DEFAULT_COL_INDEX;

            has_r = true;
            has_backslash = false;
            continue;
        }
        else if (ch == '\n') {
            if (!has_backslash) {
                break;
            }

            string_builder_append_char_right(&sb, ch);

            ++lexer->chunk.pos;
            ++lexer->loc.end.pos;

            if (!has_r) {
                ++lexer->loc.end.row;
                lexer->loc.end.col = DEFAULT_COL_INDEX;
            }

            has_r = false;
            has_backslash = false;
            continue;
        }
       
        string_builder_append_char_right(&sb, ch);

        consume_char(lexer);

        has_r = false;
        has_backslash = false;
    }

    char* value = string_builder_get_str(&sb);
    string_builder_free(&sb);

    if (!is_good) {
        if (lexer->diag != NULL) {
            diagnostic_engine_report(lexer->diag, ERR_UNTERMINATED_COMMENT_BLOCK, lexer->loc);
        }

        return token_create(TOKEN_INVALID, NULL, lexer->loc);
    }

    return token_create(TOKEN_STRING_LITERAL, value, lexer->loc);
}

Token lexer_parse_char_literal(Lexer* lexer) {
    assert(lexer != NULL);

    bool has_r = false;
    bool has_backslash = false;
    bool is_good = false;

    StringBuilder sb = string_builder_create();

    char ch = '\0';
    while ((ch = get_curr_char_from_stream(lexer)) != '\0') {
        if (ch == '\\' && !has_backslash) {
            string_builder_append_char_right(&sb, ch);
            consume_char(lexer);

            has_r = false;
            has_backslash = true;
            continue;
        }
        else if (ch == '\'' && !has_backslash) {
            consume_char(lexer);

            is_good = true;
            break;
        }
        else if (ch == '\r') {
            if (!has_backslash) {
                break;
            }

            string_builder_append_char_right(&sb, ch);

            ++lexer->chunk.pos;
            ++lexer->loc.end.pos;
            ++lexer->loc.end.row;
            lexer->loc.end.col = DEFAULT_COL_INDEX;

            has_r = true;
            has_backslash = false;
            continue;
        }
        else if (ch == '\n') {
            if (!has_backslash) {
                break;
            }

            string_builder_append_char_right(&sb, ch);

            ++lexer->chunk.pos;
            ++lexer->loc.end.pos;

            if (!has_r) {
                ++lexer->loc.end.row;
                lexer->loc.end.col = DEFAULT_COL_INDEX;
            }

            has_r = false;
            has_backslash = false;
            continue;
        }

        string_builder_append_char_right(&sb, ch);

        consume_char(lexer);

        has_r = false;
        has_backslash = false;
    }

    // There is no terminating character for char literal
    if (!is_good) {
        string_builder_free(&sb);

        if (lexer->diag != NULL) {
            diagnostic_engine_report(lexer->diag, ERR_UNTERMINATED_CHAR_LITERAL, lexer->loc);
        }
        return token_create(TOKEN_INVALID, NULL, lexer->loc);
    }
    // There is no characters in char literal body
    else if (sb.buffer.items_count == 0) {
        string_builder_free(&sb);

        if (lexer->diag != NULL) {
            diagnostic_engine_report(lexer->diag, ERR_CHAR_LITERAL_HAS_NO_BODY, lexer->loc);
        }
        return token_create(TOKEN_INVALID, NULL, lexer->loc);
    }
    else if (sb.buffer.items_count >= 2 && ((char*)sb.buffer.items)[0] != '\\') {
        string_builder_free(&sb);

        if (lexer->diag != NULL) {
            diagnostic_engine_report(lexer->diag, ERR_CHAR_LITERAL_HAS_TOO_MANY_CHARACTERS, lexer->loc);
        }
        return token_create(TOKEN_INVALID, NULL, lexer->loc);
    }

    char* value = string_builder_get_str(&sb);
    string_builder_free(&sb);

    return token_create(TOKEN_CHAR_LITERAL, value, lexer->loc);
}

Token lexer_parse_hex_literal(Lexer* lexer) {
    assert(lexer != NULL);

    bool is_good = true;

    StringBuilder sb = string_builder_create();
    string_builder_append_str_right(&sb, "0x");

    char ch = '\0';
    while ((ch = get_curr_char_from_stream(lexer)) != '\0') {
        if (is_punct(ch) || is_space(ch)) {
            break;
        }
        else if (!is_xdigit(ch)) {
            is_good = false;
        }

        string_builder_append_char_right(&sb, ch);
        consume_char(lexer);
    }
    // 0x
    if (sb.buffer.items_count == 2) {
        string_builder_free(&sb);

        if (lexer->diag != NULL) {
            diagnostic_engine_report(lexer->diag, ERR_LITERAL_HAS_NO_BODY, lexer->loc);
        }
        return token_create(TOKEN_INVALID, NULL, lexer->loc);
    }

    char* value = string_builder_get_str(&sb);
    string_builder_free(&sb);

    if (!is_good) {
        if (lexer->diag != NULL) {
            diagnostic_engine_report(lexer->diag, ERR_INVALID_CHARACTERS_IN_LITERAL, lexer->loc, value);
        }

        return token_create(TOKEN_INVALID, value, lexer->loc);
    }

    return token_create(TOKEN_HEX_LITERAL, value, lexer->loc);
}

Token lexer_parse_bits_literal(Lexer* lexer) {
    assert(lexer != NULL);

    bool is_good = true;

    StringBuilder sb = string_builder_create();
    string_builder_append_str_right(&sb, "0b");

    char ch = '\0';
    while ((ch = get_curr_char_from_stream(lexer)) != '\0') {
        if (is_punct(ch) || is_space(ch)) {
            break;
        }
        else if (!is_bdigit(ch)) {
            is_good = false;
        }

        string_builder_append_char_right(&sb, ch);
        consume_char(lexer);
    }
    // 0b
    if (sb.buffer.items_count == 2) {
        string_builder_free(&sb);

        if (lexer->diag != NULL) {
            diagnostic_engine_report(lexer->diag, ERR_LITERAL_HAS_NO_BODY, lexer->loc);
        }
        return token_create(TOKEN_INVALID, NULL, lexer->loc);
    }

    char* value = string_builder_get_str(&sb);
    string_builder_free(&sb);

    if (!is_good) {
        if (lexer->diag != NULL) {
            diagnostic_engine_report(lexer->diag, ERR_INVALID_CHARACTERS_IN_LITERAL, lexer->loc, value);
        }

        return token_create(TOKEN_INVALID, value, lexer->loc);
    }

    return token_create(TOKEN_BITS_LITERAL, value, lexer->loc);
}

Token lexer_parse_dec_or_oct_literal(Lexer* lexer, const char prev_ch) {
    assert(lexer != NULL);

    bool is_good = true;

    StringBuilder sb = string_builder_create();
    string_builder_append_char_right(&sb, prev_ch);

    char ch = '\0';
    while ((ch = get_curr_char_from_stream(lexer)) != '\0') {
        if (is_punct(ch) || is_space(ch)) {
            break;
        }
        else if (prev_ch == '0' && !is_odigit(ch)) {
            is_good = false;
        }
        else if (!is_digit(ch)) {
            is_good = false;
        }
        string_builder_append_char_right(&sb, ch);
        consume_char(lexer);
    }

    char* value = string_builder_get_str(&sb);
    u64 len = sb.buffer.items_count;
    string_builder_free(&sb);

    if (prev_ch == '0') {
        if (len == 1) {
            return token_create(TOKEN_DEC_LITERAL, value, lexer->loc);
        }
        else if (!is_good) {
            if (lexer->diag != NULL) {
                diagnostic_engine_report(lexer->diag, ERR_INVALID_CHARACTERS_IN_LITERAL, lexer->loc, value);
            }
            return token_create(TOKEN_INVALID, value, lexer->loc);
        }
        return token_create(TOKEN_OCT_LITERAL, value, lexer->loc);
    }
    // dec
    if (!is_good) {
        if (lexer->diag != NULL) {
            diagnostic_engine_report(lexer->diag, ERR_INVALID_CHARACTERS_IN_LITERAL, lexer->loc, value);
        }
        return token_create(TOKEN_INVALID, value, lexer->loc);
    }
    return token_create(TOKEN_DEC_LITERAL, value, lexer->loc);
}

Token lexer_parse_next_token(Lexer* lexer) {
    assert(lexer != NULL);

    skip_whitespaces(lexer);

    const char c = consume_char(lexer);

    switch (c) {
    case '\0':  return token_create(TOKEN_END_OF_FILE, NULL, lexer->loc);
    case '(':   return token_create(TOKEN_L_BRACE, NULL, lexer->loc);
    case ')':   return token_create(TOKEN_R_BRACE, NULL, lexer->loc);
    case ',':   return token_create(TOKEN_COMMA, NULL, lexer->loc);
    case '?':   return token_create(TOKEN_QUESTION, NULL, lexer->loc);
    case ';':   return token_create(TOKEN_SEMICOLON, NULL, lexer->loc);
    case ':':   return token_create(TOKEN_COLON, NULL, lexer->loc);
    case '+':   return lexer_parse_c2(lexer, '+', '=', TOKEN_PLUS, TOKEN_PLUS_PLUS, TOKEN_PLUS_EQUAL);
    case '-':   return lexer_parse_c2(lexer, '-', '=', TOKEN_MINUS, TOKEN_MINUS_MINUS, TOKEN_MINUS_EQUAL);
    case '*':   return lexer_parse_c1(lexer, '=', TOKEN_STAR, TOKEN_STAR_EQUAL);
    case '%':   return lexer_parse_c1(lexer, '=', TOKEN_PERCENT, TOKEN_PERCENT_EQUAL);
    case '/': {
        char tmp_c = get_curr_char_from_stream(lexer);

        if (tmp_c == '=') {
            consume_char(lexer);
            return token_create(TOKEN_SLASH_EQUAL, NULL, lexer->loc);
        }
        else if (tmp_c == '/') {
            consume_char(lexer);
            skip_single_line_comment(lexer);
            return lexer_parse_next_token(lexer);
        }
        else if (tmp_c == '*') {
            consume_char(lexer);
            if (!skip_multi_line_comment(lexer)) {
                return token_create(TOKEN_INVALID, NULL, lexer->loc);
            }
            return lexer_parse_next_token(lexer);
        }

        return token_create(TOKEN_SLASH, NULL, lexer->loc);
    } break;
    case '>':   return lexer_parse_c2c1(lexer, '=', '>', '=', TOKEN_GREATER, TOKEN_GREATER_EQUAL, TOKEN_GREATER_GREATER, TOKEN_GREATER_GREATER_EQUAL);
    case '<':   return lexer_parse_c2c1(lexer, '=', '<', '=', TOKEN_LESS, TOKEN_LESS_EQUAL, TOKEN_LESS_LESS, TOKEN_LESS_LESS_EQUAL);
    case '^':   return lexer_parse_c1(lexer, '=', TOKEN_CARET, TOKEN_CARET_EQUAL);
    case '&':   return lexer_parse_c2(lexer, '&', '=', TOKEN_AMP, TOKEN_AMP_AMP, TOKEN_AMP_EQUAL);
    case '|':   return lexer_parse_c2(lexer, '|', '=', TOKEN_PIPE, TOKEN_PIPE_PIPE, TOKEN_PIPE_EQUAL);
    case '~':   return token_create(TOKEN_TILDE, NULL, lexer->loc);
    case '!':   return lexer_parse_c1(lexer, '=', TOKEN_EXCLAIM, TOKEN_EXCLAIM_EQUAL);
    case '=':   return lexer_parse_c1(lexer, '=', TOKEN_EQUAL, TOKEN_EQUAL_EQUAL);
    case '\"':  return lexer_parse_string_literal(lexer);
    case '\'':  return lexer_parse_char_literal(lexer);
    case '0': {
        char tmp_ch = get_curr_char_from_stream(lexer);
        // HEX
        if (tmp_ch == 'x' || tmp_ch == 'X') {
            consume_char(lexer);
            return lexer_parse_hex_literal(lexer);
        }
        // BITS
        else if (tmp_ch == 'b' || tmp_ch == 'B') {
            consume_char(lexer);
            return lexer_parse_bits_literal(lexer);
        }
        // OCT OR DEC
        return lexer_parse_dec_or_oct_literal(lexer, c);
    } break;
    case '1':   return lexer_parse_dec_or_oct_literal(lexer, c);
    case '2':   return lexer_parse_dec_or_oct_literal(lexer, c);
    case '3':   return lexer_parse_dec_or_oct_literal(lexer, c);
    case '4':   return lexer_parse_dec_or_oct_literal(lexer, c);
    case '5':   return lexer_parse_dec_or_oct_literal(lexer, c);
    case '6':   return lexer_parse_dec_or_oct_literal(lexer, c);
    case '7':   return lexer_parse_dec_or_oct_literal(lexer, c);
    case '8':   return lexer_parse_dec_or_oct_literal(lexer, c);
    case '9':   return lexer_parse_dec_or_oct_literal(lexer, c);
    default: {
        StringBuilder sb = string_builder_create();
        string_builder_append_char_right(&sb, c);

        if (!is_alpha(c) && c != '_') {
            char* value = string_builder_get_str(&sb);
            string_builder_free(&sb);

            if (lexer->diag != NULL) {
                diagnostic_engine_report(lexer->diag, ERR_UNKNOWN_CHARACTER, lexer->loc, value);
            }

            return token_create(TOKEN_INVALID, value, lexer->loc);
        }

        char ch = '\0';
        do {
            ch = get_curr_char_from_stream(lexer);
            if (!is_alpha(ch) && !is_digit(ch) && ch != '_') {
                break;
            }

            string_builder_append_char_right(&sb, ch);
            consume_char(lexer);
        } while (ch != '\0');

        char* value = string_builder_get_str(&sb);
        string_builder_free(&sb);

        // bool literal
        if (str_eq(value, "true") || str_eq(value, "false")) {
            return token_create(TOKEN_BOOL_LITERAL, value, lexer->loc);
        }

#define RETURN_IF_KEYWORD(keyword, kind)            \
if (str_eq(value, keyword)) {                       \
    free(value);                                    \
    return token_create(kind, NULL, lexer->loc);    \
}   
        // literals
        if (str_eq(value, "true")) { return token_create(TOKEN_BITS_LITERAL, value, lexer->loc); }
        else if (str_eq(value, "false")) { return token_create(TOKEN_BITS_LITERAL, value, lexer->loc); }
        // keywords
        else RETURN_IF_KEYWORD("function", TOKEN_FUNCTION_KEYWORD)
        else RETURN_IF_KEYWORD("end", TOKEN_END_KEYWORD)
        else RETURN_IF_KEYWORD("as", TOKEN_AS_KEYWORD)
        else RETURN_IF_KEYWORD("dim", TOKEN_DIM_KEYWORD)
        else RETURN_IF_KEYWORD("if", TOKEN_IF_KEYWORD)
        else RETURN_IF_KEYWORD("then", TOKEN_THEN_KEYWORD)
        else RETURN_IF_KEYWORD("else", TOKEN_ELSE_KEYWORD)
        else RETURN_IF_KEYWORD("while", TOKEN_WHILE_KEYWORD)
        else RETURN_IF_KEYWORD("wend", TOKEN_WEND_KEYWORD)
        else RETURN_IF_KEYWORD("do", TOKEN_DO_KEYWORD)
        else RETURN_IF_KEYWORD("loop", TOKEN_LOOP_KEYWORD)
        else RETURN_IF_KEYWORD("until", TOKEN_UNTIL_KEYWORD)
        else RETURN_IF_KEYWORD("break", TOKEN_BREAK_KEYWORD)
        else RETURN_IF_KEYWORD("continue", TOKEN_CONTINUE_KEYWORD)
        // builtins 
        else RETURN_IF_KEYWORD("bool", TOKEN_BOOL_BUILTIN)
        else RETURN_IF_KEYWORD("byte", TOKEN_BYTE_BUILTIN)
        else RETURN_IF_KEYWORD("int", TOKEN_INT_BUILTIN)
        else RETURN_IF_KEYWORD("uint", TOKEN_UINT_BUILTIN)
        else RETURN_IF_KEYWORD("long", TOKEN_LONG_BUILTIN)
        else RETURN_IF_KEYWORD("ulong", TOKEN_ULONG_BUILTIN)
        else RETURN_IF_KEYWORD("char", TOKEN_CHAR_BUILTIN)
        else RETURN_IF_KEYWORD("string", TOKEN_STRING_BUILTIN)

        return token_create(TOKEN_IDENTIFIER, value, lexer->loc);
    } break;
    };
}