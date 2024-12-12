#include "vanec/frontend/cfg/cfg_context.h"

#include <assert.h>
#include <stdlib.h>

CFGContext* cfg_context_create(DiagnosticEngine* diag) {
    CFGContext* ctx = malloc(sizeof(CFGContext));
    assert(ctx != NULL);

    ctx->diag = diag;
    ctx->curr_scope = NULL;

    ctx->nodes = vector_create(DEFAULT_VECTOR_CAPACITY, sizeof(CFGNode*), &cfg_node_free, true);
    ctx->scopes = vector_create(DEFAULT_VECTOR_CAPACITY, sizeof(CFGScope*), &cfg_scope_free, true);

    return ctx;
}

void cfg_context_free(CFGContext* ctx) {
    if (ctx == NULL) {
        return;
    }

    vector_free(&ctx->scopes);
    vector_free(&ctx->nodes);
    free(ctx);
}

void cfg_context_clear(CFGContext* ctx) {
    assert(ctx != NULL);

    vector_clear(&ctx->scopes);
    vector_clear(&ctx->nodes);
}

void cfg_context_enter_scope(CFGContext* ctx, const CFGScopeKind kind) {
    assert(ctx != NULL);

    CFGScope* scope = cfg_scope_create(kind, ctx->curr_scope);
    vector_push_back(&ctx->scopes, &scope);

    ctx->curr_scope = scope;
}

void cfg_context_leave_scope(CFGContext* ctx) {
    assert(ctx != NULL);

    if (ctx->curr_scope == NULL) {
        return;
    }

    ctx->curr_scope = ctx->curr_scope->prev;
}

CFGNode* cfg_context_create_cfg_node(CFGContext* ctx, const CFGNodeKind kind) {
    assert(ctx != NULL);

    CFGNode* node = cfg_node_create(kind, (u32)ctx->nodes.items_count, ctx->curr_scope);
    vector_push_back(&ctx->nodes, &node);

    return node;
}