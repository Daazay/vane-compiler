#pragma once

#include "vanec/utils/defines.h"
#include "vanec/utils/vector.h"

typedef struct CFGNode CFGNode;

typedef enum {
    CFG_UNKNOWN_SCOPE   = 0,
    CFG_FUNCTION_SCOPE  = 1,
    CFG_BASIC_SCOPE     = 2,
    CFG_LOOP_SCOPE      = 3,
} CFGScopeKind;

typedef struct CFGScope CFGScope;

struct CFGScope {
    CFGScopeKind kind;

    CFGScope* prev;
    bool has_break;

    CFGNode* break_node;
    CFGNode* backedge_node;
};

CFGScope* cfg_scope_create(const CFGScopeKind kind, CFGScope* prev);

void cfg_scope_free(CFGScope* scope);