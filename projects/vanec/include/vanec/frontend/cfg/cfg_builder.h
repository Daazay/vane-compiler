#pragma once

#include "vanec/frontend/ast/ast_node.h"
#include "vanec/frontend/cfg/cfg_node.h"
#include "vanec/frontend/cfg/cfg_context.h"

CFGNode* build_cfg_for_function(CFGContext* ctx, const ASTNode* ast);

bool build_cfg_for_statements_block(CFGContext* ctx, const Vector* block, CFGNode** first, CFGNode** last);

bool build_cfg_for_statement(CFGContext* ctx, const ASTNode* ast, CFGNode** first, CFGNode** last);

bool prepare_basic_cfg_node(CFGContext* ctx, const ASTNode* ast, CFGNode** first, CFGNode** last);

bool prepare_condition_cfg_node(CFGContext* ctx, const ASTNode* ast, CFGNode** first, CFGNode** last);

bool prepare_while_loop_cfg_node(CFGContext* ctx, const ASTNode* ast, CFGNode** first, CFGNode** last);

bool prepare_do_loop_cfg_node(CFGContext* ctx, const ASTNode* ast, CFGNode** first, CFGNode** last);

bool handle_break_stmt_in_cfg(CFGContext* ctx, const ASTNode* ast, CFGNode** first, CFGNode** last);

bool handle_continue_stmt_in_cfg(CFGContext* ctx, const ASTNode* ast, CFGNode** first, CFGNode** last);

bool handle_return_stmt_in_cfg(CFGContext* ctx, const ASTNode* ast, CFGNode** first, CFGNode** last);