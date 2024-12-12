#include "vanec/frontend/cfg/cfg_node.h"

#include <assert.h>
#include <stdlib.h>

#define ALLOCATE_CFG_NODE_DATA(data, kind)  \
data = calloc(1, sizeof(kind));             \
assert(data != NULL)

CFGNode* cfg_node_create(const CFGNodeKind kind, const u32 id, const CFGScope* scope) {
    CFGNode* node = malloc(sizeof(CFGNode));
    assert(node != NULL);

    node->kind = kind;
    node->scope = scope;
    node->id = id;
    node->visited = 0;

    switch (kind) {
    case CFG_FUNC_ENTRY_NODE: { ALLOCATE_CFG_NODE_DATA(node->as.func_entry, struct CFGFunctionData); } break;
    case CFG_FUNC_EXIT_NODE: { /* DO NOTHING */ } break;
    case CFG_BASIC_BLOCK_NODE: { ALLOCATE_CFG_NODE_DATA(node->as.basic_block, struct CFGBasicData); } break;
    case CFG_CONDITION_NODE: { ALLOCATE_CFG_NODE_DATA(node->as.condition, struct CFGConditionData); } break;
    case CFG_LOOP_ENTRY_NODE: { ALLOCATE_CFG_NODE_DATA(node->as.loop_entry, struct CFGLoopEntryData); } break;
    case CFG_LOOP_EXIT_NODE: { ALLOCATE_CFG_NODE_DATA(node->as.loop_exit, struct CFGBasicData); } break;
    case CFG_BACKEDGE_NODE: { ALLOCATE_CFG_NODE_DATA(node->as.backedge, struct CFGBasicData); } break;
    case CFG_BREAK_NODE: { ALLOCATE_CFG_NODE_DATA(node->as.break_, struct CFGBasicData); } break;
    default: {
        assert(false && "Unreachable");
    } break;
    };

    return node;
}

void cfg_node_free(CFGNode* node) {
    if (node == NULL) {
        return;
    }

    switch (node->kind) {
    case CFG_FUNC_ENTRY_NODE: {
        free(node->as.func_entry);
    } break;
    case CFG_FUNC_EXIT_NODE: { /* DO NOTHING */ } break;
    case CFG_BASIC_BLOCK_NODE: {
        free(node->as.basic_block);
    } break;
    case CFG_CONDITION_NODE: {
        free(node->as.condition);
    } break;
    case CFG_LOOP_ENTRY_NODE: {
        free(node->as.loop_entry);
    } break;
    case CFG_LOOP_EXIT_NODE: {
        free(node->as.loop_exit);
    } break;
    case CFG_BACKEDGE_NODE: {
        free(node->as.backedge);
    } break;
    case CFG_BREAK_NODE: {
        free(node->as.break_);
    } break;
    default: {
        assert(false && "Unknown node kind.");
    } break;
    };

    free(node);
}

CFGNode* chain_cfg_nodes(CFGNode* prev, CFGNode* next) {
    if (prev == NULL) {
        return next;
    }

    switch (prev->kind) {
    case CFG_FUNC_ENTRY_NODE: {
        if (prev->as.func_entry->block.next == NULL) {
            prev->as.func_entry->block.next = prev->as.func_entry->block.next = next;
        }
        else {
            chain_cfg_nodes(prev->as.func_entry->block.last , next);
        }
    }break;
    case CFG_FUNC_EXIT_NODE: {
        assert(false && "It is not possible to add any node after function exit node.");
    } break;
    case CFG_BASIC_BLOCK_NODE: {
        if (prev->as.basic_block->next == NULL) {
            prev->as.basic_block->next = next;
        }
        else {
            chain_cfg_nodes(prev->as.basic_block->next, next);
        }
    } break;
    case CFG_CONDITION_NODE: {
        if (prev->as.condition->then_branch.next == NULL) {
            prev->as.condition->then_branch.last = prev->as.condition->then_branch.next = next;
        }
        else if (prev->as.condition->then_branch.last->kind != CFG_BACKEDGE_NODE && prev->as.condition->then_branch.last->kind != CFG_BREAK_NODE) {
            chain_cfg_nodes(prev->as.condition->then_branch.last, next);
        }
        //
        if (prev->as.condition->else_branch.next == NULL) {
            prev->as.condition->else_branch.last = prev->as.condition->else_branch.next = next;
        }
        else if (prev->as.condition->else_branch.last->kind != CFG_BACKEDGE_NODE && prev->as.condition->else_branch.last->kind != CFG_BREAK_NODE) {
            chain_cfg_nodes(prev->as.condition->else_branch.last, next);
        }
    } break;
    case CFG_LOOP_ENTRY_NODE: {
        if (prev->as.loop_entry->exit == NULL) {
            prev->as.loop_entry->exit = next;
        }
        else {
            chain_cfg_nodes(prev->as.loop_entry->exit, next);
        }
    } break;
    case CFG_LOOP_EXIT_NODE: {
        if (prev->as.loop_exit->next == NULL) {
            prev->as.loop_exit->next = next;
        }
        else {
            chain_cfg_nodes(prev->as.loop_exit->next, next);
        }
    } break;
    case CFG_BACKEDGE_NODE: {
        /* DO NOTHING, CAUSE IT'S DONE MANUALY */
        assert(false && "Must not fall in here.");
    } break;
    case CFG_BREAK_NODE: {
        /* DO NOTHING, CAUSE IT'S DONE MANUALY */
        assert(false && "Must not fall in here.");
    } break;    
    default: {
        assert(false && "Unknown node kind.");
    } break;
    };

    return next;
}