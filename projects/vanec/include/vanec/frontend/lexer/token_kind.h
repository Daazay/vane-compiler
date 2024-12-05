#pragma once

#include "vanec/utils/source_loc.h"

typedef enum {
#define TOKEN(ID, NAME, VALUE) TOKEN_##ID,
#include "vanec/frontend/lexer/token_kind.def"
} TokenKind;

typedef enum {
    PREC_NONE            = 0,   // not a binary operator
    PREC_COMMA           = 1,   // ','
    PREC_ASSIGNMENT      = 2,   // '='|'+='|'-='|'*='|'/='|'%='|'>>='|'<<='|'^='
    PREC_TERNARY         = 3,   // '?'
    PREC_LOGICAL_OR      = 4,   // '||'
    PREC_LOGICAL_AND     = 5,   // '&&'
    PREC_BIT_OR          = 6,   // '|'
    PREC_BIT_XOR         = 7,   // '^'
    PREC_BIT_AND         = 8,   // '&'
    PREC_EQUALITY        = 9,   // '=='|'!='
    PREC_RELATIONAL      = 10,  // '>'|'>='|'<'|'<='
    PREC_ADDITIVE        = 12,  // '+'|'-'
    PREC_SHIFT           = 11,  // '>>'|'<<'
    PREC_MULTIPLICATIVE  = 13,  // '*'|'/'|'%'
    PREC_CALL            = 14,  // '('
    PREC_UNARY           = 15,  // '+'|'++'|'-'|'--'|'^'|'~'
} Precedence;

typedef enum {
    ASSOC_LEFT_TO_RIGHT = 0,
    ASSOC_RIGHT_TO_LEFT = 1,
} Associativity;

const char* get_token_kind_spelling(const TokenKind kind);

const char* get_token_kind_value(const TokenKind kind);

Precedence get_precedence(const TokenKind kind);

Associativity get_associativity(const Precedence prec);