#pragma once

typedef enum {
    CFG_UNKNOWN_NODE     = 0,
    CFG_FUNC_ENTRY_NODE  = 1,
    CFG_FUNC_EXIT_NODE   = 2,
    CFG_BASIC_BLOCK_NODE = 3,
    CFG_CONDITION_NODE   = 4,
    CFG_LOOP_ENTRY_NODE  = 5,
    CFG_LOOP_EXIT_NODE   = 6,
    CFG_BACKEDGE_NODE    = 7,
    CFG_BREAK_NODE       = 8,
} CFGNodeKind;

typedef enum {
    OP_UNKNOWN_NODE     = 0,
    OP_READ_NODE        = 1,    // x
    OP_READ_IDX_NODE    = 2,    // x(idx)
    OP_WRITE_NODE       = 3,    // 'x = y'
    OP_WRITE_IDX_NODE   = 4,    // 'x(idx) = y'
    OP_SUM_NODE         = 5,    // '+'
    OP_SUB_NODE         = 6,    // '-'
    OP_MUL_NODE         = 7,    // '*'
    OP_DIV_NODE         = 8,    // '/'
    OP_REM_NODE         = 9,    // '%'
    OP_BITS_AND_NODE    = 10,   // '&'
    OP_BITS_OR_NODE     = 11,   // '|'
    OP_BITS_XOR_NODE    = 12,   // '^'
    OP_BITS_NEG_NODE    = 13,   // '~'
    OP_BITS_LSHIFT_NODE = 14,   // '<<'
    OP_BITS_RSHIFT_NODE = 15,   // '>>'
    OP_BOOL_AND_NODE    = 16,   // '&&'
    OP_BOOL_OR_NODE     = 17,   // '||'
    OP_BOOL_NOT_NODE    = 18,   // '!'
} OpNodeKind;