#include "vanec/frontend/lexer/token_kind.h"

const char* get_token_kind_spelling(const TokenKind kind) {
	switch (kind) {
#define TOKEN(ID, NAME, VALUE) case TOKEN_##ID: return NAME;
#include "vanec/frontend/lexer/token_kind.def"
	};
	return NULL;
}

const char* get_token_kind_value(const TokenKind kind) {
	switch (kind) {
#define TOKEN(ID, NAME, VALUE) case TOKEN_##ID: return VALUE;
#include "vanec/frontend/lexer/token_kind.def"
	};
	return NULL;
}

bool is_token_kind_a_punctuator(const TokenKind kind) {
    return
        kind == TOKEN_L_BRACE ||
        kind == TOKEN_R_BRACE ||
        kind == TOKEN_COMMA ||
        kind == TOKEN_SEMICOLON ||
        kind == TOKEN_PLUS ||
        kind == TOKEN_PLUS_PLUS ||
        kind == TOKEN_PLUS_EQUAL ||
        kind == TOKEN_MINUS ||
        kind == TOKEN_MINUS_MINUS ||
        kind == TOKEN_MINUS_EQUAL ||
        kind == TOKEN_STAR ||
        kind == TOKEN_STAR_EQUAL ||
        kind == TOKEN_SLASH ||
        kind == TOKEN_SLASH_EQUAL ||
        kind == TOKEN_PERCENT ||
        kind == TOKEN_PERCENT_EQUAL ||
        kind == TOKEN_GREATER ||
        kind == TOKEN_GREATER_GREATER ||
        kind == TOKEN_GREATER_EQUAL ||
        kind == TOKEN_GREATER_GREATER_EQUAL ||
        kind == TOKEN_LESS ||
        kind == TOKEN_LESS_LESS ||
        kind == TOKEN_LESS_EQUAL ||
        kind == TOKEN_LESS_LESS_EQUAL ||
        kind == TOKEN_CARET ||
        kind == TOKEN_CARET_EQUAL ||
        kind == TOKEN_AMP ||
        kind == TOKEN_AMP_AMP ||
        kind == TOKEN_AMP_EQUAL ||
        kind == TOKEN_PIPE ||
        kind == TOKEN_PIPE_PIPE ||
        kind == TOKEN_PIPE_EQUAL ||
        kind == TOKEN_TILDE ||
        kind == TOKEN_EXCLAIM ||
        kind == TOKEN_EXCLAIM_EQUAL ||
        kind == TOKEN_EQUAL ||
        kind == TOKEN_EQUAL_EQUAL;
}

bool is_token_kind_a_literal(const TokenKind kind) {
    return
        kind == TOKEN_STRING_LITERAL ||
        kind == TOKEN_CHAR_LITERAL ||
        kind == TOKEN_DEC_LITERAL ||
        kind == TOKEN_HEX_LITERAL ||
        kind == TOKEN_OCT_LITERAL ||
        kind == TOKEN_BITS_LITERAL ||
        kind == TOKEN_BOOL_LITERAL;
}

bool is_token_kind_a_keyword(const TokenKind kind) {
    return
        kind == TOKEN_FUNCTION_KEYWORD ||
        kind == TOKEN_END_KEYWORD ||
        kind == TOKEN_AS_KEYWORD ||
        kind == TOKEN_DIM_KEYWORD ||
        kind == TOKEN_IF_KEYWORD ||
        kind == TOKEN_THEN_KEYWORD ||
        kind == TOKEN_ELSE_KEYWORD ||
        kind == TOKEN_WHILE_KEYWORD ||
        kind == TOKEN_WEND_KEYWORD ||
        kind == TOKEN_DO_KEYWORD ||
        kind == TOKEN_LOOP_KEYWORD ||
        kind == TOKEN_UNTIL_KEYWORD ||
        kind == TOKEN_BREAK_KEYWORD ||
        kind == TOKEN_CONTINUE_KEYWORD ||
        kind == TOKEN_RETURN_KEYWORD ||
        is_token_kind_a_builtin(kind);
}

bool is_token_kind_a_builtin(const TokenKind kind) {
    return
        kind == TOKEN_BOOL_BUILTIN ||
        kind == TOKEN_BYTE_BUILTIN ||
        kind == TOKEN_INT_BUILTIN ||
        kind == TOKEN_UINT_BUILTIN ||
        kind == TOKEN_LONG_BUILTIN ||
        kind == TOKEN_ULONG_BUILTIN ||
        kind == TOKEN_CHAR_BUILTIN ||
        kind == TOKEN_STRING_BUILTIN;
}

bool is_token_kind_a_binary_op(const TokenKind kind) {
    return
        kind == TOKEN_PLUS ||
        kind == TOKEN_PLUS_EQUAL ||
        kind == TOKEN_MINUS ||
        kind == TOKEN_MINUS_EQUAL ||
        kind == TOKEN_STAR ||
        kind == TOKEN_STAR_EQUAL ||
        kind == TOKEN_SLASH ||
        kind == TOKEN_SLASH_EQUAL ||
        kind == TOKEN_PERCENT ||
        kind == TOKEN_PERCENT_EQUAL ||
        kind == TOKEN_GREATER ||
        kind == TOKEN_GREATER_GREATER ||
        kind == TOKEN_GREATER_EQUAL ||
        kind == TOKEN_GREATER_GREATER_EQUAL ||
        kind == TOKEN_LESS ||
        kind == TOKEN_LESS_LESS ||
        kind == TOKEN_LESS_EQUAL ||
        kind == TOKEN_LESS_LESS_EQUAL ||
        kind == TOKEN_CARET ||
        kind == TOKEN_CARET_EQUAL ||
        kind == TOKEN_AMP ||
        kind == TOKEN_AMP_AMP ||
        kind == TOKEN_AMP_EQUAL ||
        kind == TOKEN_PIPE ||
        kind == TOKEN_PIPE_PIPE ||
        kind == TOKEN_PIPE_EQUAL ||
        kind == TOKEN_EXCLAIM_EQUAL ||
        kind == TOKEN_EQUAL ||
        kind == TOKEN_EQUAL_EQUAL;
}

bool is_token_kind_an_unary_op(const TokenKind kind) {
    return
        kind == TOKEN_PLUS ||
        kind == TOKEN_PLUS_PLUS ||
        kind == TOKEN_MINUS ||
        kind == TOKEN_MINUS_MINUS ||
        kind == TOKEN_CARET ||
        kind == TOKEN_TILDE ||
        kind == TOKEN_EXCLAIM;
}

Precedence get_precedence(const TokenKind kind) {
    switch (kind) {
        /* COMMA */
    case TOKEN_COMMA:                   return PREC_COMMA;
        /* ASSIGNMENT */
    case TOKEN_EQUAL:                   return PREC_ASSIGNMENT;
    case TOKEN_PLUS_EQUAL:              return PREC_ASSIGNMENT;
    case TOKEN_MINUS_EQUAL:             return PREC_ASSIGNMENT;
    case TOKEN_STAR_EQUAL:              return PREC_ASSIGNMENT;
    case TOKEN_SLASH_EQUAL:             return PREC_ASSIGNMENT;
    case TOKEN_PERCENT_EQUAL:           return PREC_ASSIGNMENT;
    case TOKEN_GREATER_GREATER_EQUAL:   return PREC_ASSIGNMENT;
    case TOKEN_LESS_LESS_EQUAL:         return PREC_ASSIGNMENT;
    case TOKEN_CARET_EQUAL:             return PREC_ASSIGNMENT;
    case TOKEN_AMP_EQUAL:               return PREC_ASSIGNMENT;
    case TOKEN_PIPE_EQUAL:              return PREC_ASSIGNMENT;
        /* TERNARY */
    case TOKEN_QUESTION:                return PREC_TERNARY;
        /* LOGICAL OR */
    case TOKEN_PIPE_PIPE:               return PREC_LOGICAL_OR;
        /* LOGICAL AND */
    case TOKEN_AMP_AMP:                 return PREC_LOGICAL_AND;
        /* BIT OR */
    case TOKEN_PIPE:                    return PREC_BIT_OR;
        /* BIT XOR */
    case TOKEN_CARET:                   return PREC_BIT_XOR;
        /* BIT AND */
    case TOKEN_AMP:                     return PREC_BIT_AND;
        /* BIT EQUALITY */
    case TOKEN_EQUAL_EQUAL:             return PREC_EQUALITY;
    case TOKEN_EXCLAIM_EQUAL:           return PREC_EQUALITY;
        /* BIT RELATIONAL */
    case TOKEN_GREATER:                 return PREC_RELATIONAL;
    case TOKEN_GREATER_EQUAL:           return PREC_RELATIONAL;
    case TOKEN_LESS:                    return PREC_RELATIONAL;
    case TOKEN_LESS_EQUAL:              return PREC_RELATIONAL;
        /* SHIFT */
    case TOKEN_GREATER_GREATER:         return PREC_SHIFT;
    case TOKEN_LESS_LESS:               return PREC_SHIFT;
        /* ADDITIVE */
    case TOKEN_PLUS:                    return PREC_ADDITIVE;
    case TOKEN_MINUS:                   return PREC_ADDITIVE;
        /* MULTIPLICATIVE */
    case TOKEN_STAR:                    return PREC_MULTIPLICATIVE;
    case TOKEN_SLASH:                   return PREC_MULTIPLICATIVE;
    case TOKEN_PERCENT:                 return PREC_MULTIPLICATIVE;
        /* CALL */
    case TOKEN_L_BRACE:                 return PREC_CALL;
    default:                            return PREC_NONE;
    };
}

Associativity get_associativity(const Precedence prec) {
    switch (prec) {
    case PREC_NONE:                 return ASSOC_LEFT_TO_RIGHT;
    case PREC_COMMA:                return ASSOC_LEFT_TO_RIGHT;
    case PREC_ASSIGNMENT:           return ASSOC_RIGHT_TO_LEFT;
    case PREC_LOGICAL_OR:           return ASSOC_LEFT_TO_RIGHT;
    case PREC_LOGICAL_AND:          return ASSOC_LEFT_TO_RIGHT;
    case PREC_BIT_OR:               return ASSOC_LEFT_TO_RIGHT;
    case PREC_BIT_XOR:              return ASSOC_LEFT_TO_RIGHT;
    case PREC_BIT_AND:              return ASSOC_LEFT_TO_RIGHT;
    case PREC_EQUALITY:             return ASSOC_LEFT_TO_RIGHT;
    case PREC_RELATIONAL:           return ASSOC_LEFT_TO_RIGHT;
    case PREC_SHIFT:                return ASSOC_LEFT_TO_RIGHT;
    case PREC_ADDITIVE:             return ASSOC_LEFT_TO_RIGHT;
    case PREC_MULTIPLICATIVE:       return ASSOC_LEFT_TO_RIGHT;
    case PREC_CALL:                 return ASSOC_LEFT_TO_RIGHT;
    case PREC_UNARY:                return ASSOC_RIGHT_TO_LEFT;
    case PREC_TERNARY:              return ASSOC_RIGHT_TO_LEFT;
    default:                        return ASSOC_LEFT_TO_RIGHT;
    };
}