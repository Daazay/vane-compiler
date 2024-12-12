#include "vanec/frontend/cfg/cfg_builder.h"

#include <assert.h>

CFGNode* build_cfg_for_function(CFGContext* ctx, const ASTNode* ast) {
    assert(ctx != NULL && ast != NULL);

    CFGNode* func_entry = cfg_context_create_cfg_node(ctx, CFG_FUNC_ENTRY_NODE);

    cfg_context_enter_scope(ctx, CFG_FUNCTION_SCOPE);

    const Vector* stmts = &ast->as.funcdef->stmts;
    if (build_cfg_for_statements_block(ctx, stmts, &func_entry->as.func_entry->block.first, &func_entry->as.func_entry->block.last)) {
        return false;
    }

    cfg_context_leave_scope(ctx);

    CFGNode* func_exit = cfg_context_create_cfg_node(ctx, CFG_FUNC_EXIT_NODE);

    if (func_entry->as.func_entry->block.first == NULL) {
        func_entry->as.func_entry->block.first = func_exit;
    }
    func_entry->as.func_entry->block.last = chain_cfg_nodes(func_entry->as.func_entry->block.last, func_exit);

    return func_entry;
}

bool build_cfg_for_statements_block(CFGContext* ctx, const Vector* block, CFGNode** first, CFGNode** last) {
    assert(ctx != NULL && block != NULL && first != NULL && last != NULL);

    for (u64 i = 0; i < block->items_count; ++i) {
        const ASTNode* stmt = vector_get_ref(block, i);

        if (!build_cfg_for_statement(ctx, stmt, first, last)) {
            return false;
        }
        if ((i + 1 < block->items_count) && (stmt->kind == AST_BREAK_STMT_NODE || stmt->kind == AST_CONTINUE_STMT_NODE)) {
            if (ctx->diag != NULL) {
                const ASTNode* next_stmt = vector_get_ref(block, i + 1);
                diagnostic_engine_report(ctx->diag, ERR_UNREACHABLE_CODE, next_stmt->loc);
            }
            break;
        }
    }

    return true;
}

bool build_cfg_for_statement(CFGContext* ctx, const ASTNode* ast, CFGNode** first, CFGNode** last) {
    assert(ctx != NULL && ast != NULL && first != NULL && last != NULL);

    switch (ast->kind) {
    case AST_VARDECL_STMT_NODE: { return prepare_basic_cfg_node(ctx, ast, first, last); } break;
    case AST_EXPRESSION_STMT_NODE: { return prepare_basic_cfg_node(ctx, ast, first, last); } break;
    case AST_CONDITION_STMT_NODE: { return prepare_condition_cfg_node(ctx, ast, first, last); } break;
    case AST_WHILE_STMT_NODE: { return prepare_while_loop_cfg_node(ctx, ast, first, last); } break;
    case AST_DO_WHILE_STMT_NODE: { return prepare_do_loop_cfg_node(ctx, ast, first, last); } break;
    case AST_BREAK_STMT_NODE: { return handle_break_stmt_in_cfg(ctx, ast, first, last); } break;
    case AST_CONTINUE_STMT_NODE: { return handle_continue_stmt_in_cfg(ctx, ast, first, last); } break;
    default: {
        assert(false && "Unreachable");
    } break;
    };

    return true;
}

bool prepare_basic_cfg_node(CFGContext* ctx, const ASTNode* ast, CFGNode** first, CFGNode** last) {
    assert(ctx != NULL && ast != NULL && first != NULL && last != NULL);

    if (*last == NULL) {
        CFGNode* cfg = cfg_context_create_cfg_node(ctx, CFG_BASIC_BLOCK_NODE);
        *first = cfg;
        *last = cfg;
    }
    else if ((*last)->kind != CFG_BASIC_BLOCK_NODE) {
        CFGNode* cfg = cfg_context_create_cfg_node(ctx, CFG_BASIC_BLOCK_NODE);
        *last = chain_cfg_nodes(*last, cfg);
    }

    return true;
}

bool prepare_condition_cfg_node(CFGContext* ctx, const ASTNode* ast, CFGNode** first, CFGNode** last) {
    assert(ctx != NULL && ast != NULL && first != NULL && last != NULL);

    const ASTNode* expr = ast->as.condition_stmt->expr;

    CFGNode* condition = cfg_context_create_cfg_node(ctx, CFG_CONDITION_NODE);

    // THEN BRANCH
    cfg_context_enter_scope(ctx, CFG_BASIC_SCOPE);

    const Vector* stmts = &ast->as.condition_stmt->then_branch;
    if (!build_cfg_for_statements_block(ctx, stmts, &condition->as.condition->then_branch.first, &condition->as.condition->then_branch.last)) {
        return false;
    }

    cfg_context_leave_scope(ctx);

    // ELSE BRANCH
    cfg_context_enter_scope(ctx, CFG_BASIC_SCOPE);

    stmts = &ast->as.condition_stmt->else_branch;
    if (!build_cfg_for_statements_block(ctx, stmts, &condition->as.condition->else_branch.first, &condition->as.condition->else_branch.last)) {
        return false;
    }

    cfg_context_leave_scope(ctx);

    if (*first == NULL) {
        *first = condition;
    }
    *last = chain_cfg_nodes(*last, condition);

    return true;
}

bool prepare_while_loop_cfg_node(CFGContext* ctx, const ASTNode* ast, CFGNode** first, CFGNode** last) {
    assert(ctx != NULL && ast != NULL && first != NULL && last != NULL);

    const ASTNode* expr = ast->as.condition_stmt->expr;

    CFGNode* loop_entry = cfg_context_create_cfg_node(ctx, CFG_LOOP_ENTRY_NODE);
    CFGNode* loop_exit = cfg_context_create_cfg_node(ctx, CFG_LOOP_EXIT_NODE);
    CFGNode* condition = cfg_context_create_cfg_node(ctx, CFG_CONDITION_NODE);

    // THEN BRANCH
    cfg_context_enter_scope(ctx, CFG_LOOP_SCOPE);

    ctx->curr_scope->break_node = loop_exit;
    ctx->curr_scope->backedge_node = condition;

    const Vector* stmts = &ast->as.while_stmt->stmts;
    if (!build_cfg_for_statements_block(ctx, stmts, &condition->as.condition->then_branch.first, &condition->as.condition->then_branch.last)) {
        return false;
    }

    // ADD BACKEDGE
    if (condition->as.condition->then_branch.last == NULL) {
        CFGNode* backedge = cfg_context_create_cfg_node(ctx, CFG_BACKEDGE_NODE);
        condition->as.condition->then_branch.first = backedge;
        condition->as.condition->then_branch.last = backedge;
        backedge->as.backedge->next = condition;
    }
    else if (condition->as.condition->then_branch.last->kind != CFG_BREAK_NODE && condition->as.condition->then_branch.last->kind != CFG_BACKEDGE_NODE) {
        CFGNode* backedge = cfg_context_create_cfg_node(ctx, CFG_BACKEDGE_NODE);
        condition->as.condition->then_branch.last = chain_cfg_nodes(condition->as.condition->then_branch.last, backedge);
        backedge->as.backedge->next = condition;
    }

    cfg_context_leave_scope(ctx);

    condition->as.condition->else_branch.first = loop_exit;
    condition->as.condition->else_branch.last = loop_exit;

    loop_entry->as.loop_entry->block.first = condition;
    loop_entry->as.loop_entry->block.last = condition;
    loop_entry->as.loop_entry->exit = loop_exit;

    if (*first == NULL) {
        *first = condition;
    }
    *last = chain_cfg_nodes(*last, loop_entry);

    return true;
}

bool prepare_do_loop_cfg_node(CFGContext* ctx, const ASTNode* ast, CFGNode** first, CFGNode** last) {
    assert(ctx != NULL && ast != NULL && first != NULL && last != NULL);

    const ASTNode* expr = ast->as.condition_stmt->expr;

    CFGNode* loop_entry = cfg_context_create_cfg_node(ctx, CFG_LOOP_ENTRY_NODE);
    CFGNode* loop_exit = cfg_context_create_cfg_node(ctx, CFG_LOOP_EXIT_NODE);
    CFGNode* condition = cfg_context_create_cfg_node(ctx, CFG_CONDITION_NODE);

    // THEN BRANCH
    cfg_context_enter_scope(ctx, CFG_LOOP_SCOPE);

    ctx->curr_scope->break_node = loop_exit;
    ctx->curr_scope->backedge_node = condition;

    const Vector* stmts = &ast->as.do_while_stmt->stmts;
    if (!build_cfg_for_statements_block(ctx, stmts, &loop_entry->as.loop_entry->block.first, &loop_entry->as.loop_entry->block.last)) {
        return false;
    }

    // ADD BACKEDGE
    CFGNode* backedge = cfg_context_create_cfg_node(ctx, CFG_BACKEDGE_NODE);

    condition->as.condition->else_branch.first = loop_exit;
    condition->as.condition->else_branch.last = loop_exit;
    condition->as.condition->then_branch.first = backedge;
    condition->as.condition->then_branch.last = backedge;

    cfg_context_leave_scope(ctx);
    if (loop_entry->as.loop_entry->block.first == NULL) {
        loop_entry->as.loop_entry->block.first = condition;
        loop_entry->as.loop_entry->block.last = condition;
    }
    loop_entry->as.loop_entry->block.last = chain_cfg_nodes(loop_entry->as.loop_entry->block.last, condition);
    backedge->as.backedge->next = loop_entry->as.loop_entry->block.first;

    loop_entry->as.loop_entry->exit = loop_exit;

    if (*first == NULL) {
        *first = condition;
    }
    *last = chain_cfg_nodes(*last, loop_entry);

    return true;
}

bool handle_break_stmt_in_cfg(CFGContext* ctx, const ASTNode* ast, CFGNode** first, CFGNode** last) {
    assert(ctx != NULL && ast != NULL && first != NULL && last != NULL);

    CFGScope* scope_it = ctx->curr_scope;
    while (scope_it != NULL && scope_it->kind != CFG_LOOP_SCOPE) {
        scope_it = scope_it->prev;
    }

    if (scope_it == NULL) {
        if (ctx->diag != NULL) {
            diagnostic_engine_report(ctx->diag, ERR_INVALID_BREAK_USAGE, ast->loc);
        }
        return false;
    }

    CFGNode* cfg = cfg_context_create_cfg_node(ctx, CFG_BREAK_NODE);
    cfg->as.break_->next = scope_it->break_node;
    scope_it->has_break = true;

    if (*first == NULL) {
        *first = cfg;
    }
    *last = chain_cfg_nodes(*last, cfg);

    return true;
}

bool handle_continue_stmt_in_cfg(CFGContext* ctx, const ASTNode* ast, CFGNode** first, CFGNode** last) {
    assert(ctx != NULL && ast != NULL && first != NULL && last != NULL);

    CFGScope* scope_it = ctx->curr_scope;
    while (scope_it != NULL && scope_it->kind != CFG_LOOP_SCOPE) {
        scope_it = scope_it->prev;
    }

    if (scope_it == NULL) {
        if (ctx->diag != NULL) {
            diagnostic_engine_report(ctx->diag, ERR_INVALID_CONTINUE_USAGE, ast->loc);
        }
        return false;
    }

    CFGNode* cfg = cfg_context_create_cfg_node(ctx, CFG_BACKEDGE_NODE);
    cfg->as.backedge->next = scope_it->backedge_node;
    scope_it->has_break = true;

    if (*first == NULL) {
        *first = cfg;
    }
    *last = chain_cfg_nodes(*last, cfg);

    return true;
}