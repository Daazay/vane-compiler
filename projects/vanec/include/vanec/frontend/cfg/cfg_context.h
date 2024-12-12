#pragma once

#include "vanec/utils/vector.h"

#include "vanec/diagnostic/diagnostic.h"

#include "vanec/frontend/cfg/cfg_node.h"
#include "vanec/frontend/cfg/cfg_scope.h"

typedef struct {
    Vector scopes;
    Vector nodes;

    CFGScope* curr_scope;
    DiagnosticEngine* diag;
} CFGContext;

CFGContext* cfg_context_create(DiagnosticEngine* diag);

void cfg_context_free(CFGContext* ctx);

void cfg_context_clear(CFGContext* ctx);

void cfg_context_enter_scope(CFGContext* ctx, const CFGScopeKind kind);

void cfg_context_leave_scope(CFGContext* ctx);

CFGNode* cfg_context_create_cfg_node(CFGContext* ctx, const CFGNodeKind kind);