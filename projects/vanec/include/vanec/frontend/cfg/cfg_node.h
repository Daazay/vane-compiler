#pragma once

#include "vanec/utils/defines.h"

#include "vanec/frontend/cfg/cfg_scope.h"

typedef enum {
    CFG_UNKNOWN_NODE = 0,
    CFG_FUNC_ENTRY_NODE = 1,
    CFG_FUNC_EXIT_NODE = 2,
    CFG_BASIC_BLOCK_NODE = 3,
    CFG_CONDITION_NODE = 4,
    CFG_LOOP_ENTRY_NODE = 5,
    CFG_LOOP_EXIT_NODE = 6,
    CFG_BACKEDGE_NODE = 7,
    CFG_BREAK_NODE = 8,
} CFGNodeKind;

typedef struct CFGNode CFGNode;

struct CFGChainData {
    union {
        CFGNode* first;
        CFGNode* next;
    };
    CFGNode* last;
};

struct CFGFunctionData {
    struct CFGChainData block;
};

struct CFGBasicData {
    CFGNode* next;
};

struct CFGConditionData {
    struct CFGChainData then_branch;
    struct CFGChainData else_branch;
};

struct CFGLoopEntryData {
    struct CFGChainData block;
    CFGNode* exit;
};

struct CFGNode {
    CFGNodeKind kind;
    u32 id;
    u32 visited;

    union {
        struct CFGFunctionData* func_entry;
        struct CFGBasicData* basic_block;
        struct CFGConditionData* condition;
        struct CFGLoopEntryData* loop_entry;
        struct CFGBasicData* loop_exit;
        struct CFGBasicData* backedge;
        struct CFGBasicData* break_;
    } as;

    const CFGScope* scope;
};

CFGNode* cfg_node_create(const CFGNodeKind kind, const u32 id, const CFGScope* scope);

void cfg_node_free(CFGNode* node);

CFGNode* chain_cfg_nodes(CFGNode* prev, CFGNode* next);