#include "vanec/frontend/cfg/cfg_scope.h"

#include <assert.h>
#include <stdlib.h>

CFGScope* cfg_scope_create(const CFGScopeKind kind, CFGScope* prev) {
    CFGScope* scope = malloc(sizeof(CFGScope));
    assert(scope != NULL);

    scope->kind = kind;
    scope->prev = prev;
    scope->has_break = false;
    scope->break_node = NULL;
    scope->backedge_node = NULL;

    return scope;
}

void cfg_scope_free(CFGScope* scope) {
    if (scope == NULL) {
        return;
    }

    free(scope);
}