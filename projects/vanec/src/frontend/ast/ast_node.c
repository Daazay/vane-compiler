#include "vanec/frontend/ast/ast_node.h"

#include <assert.h>
#include <stdlib.h>

#define ALLOC_AST_NODE_DATA(var, type)  \
var = malloc(sizeof(type));             \
assert(var)

ASTNode* ast_node_create(const ASTNodeKind kind) {
    ASTNode* node = malloc(sizeof(ASTNode));
    assert(node != NULL);

    node->kind = kind;

    switch (kind) {
    case AST_FUNCSIGN_NODE: { ALLOC_AST_NODE_DATA(node->as.funcsign, struct ASTFuncSignData); } break;
    case AST_ARGDEF_NODE: { ALLOC_AST_NODE_DATA(node->as.argdef, struct ASTArgDefData); } break;
    case AST_IDENTIFIER_NODE: { ALLOC_AST_NODE_DATA(node->as.identifier, struct ASTIdentifierData); } break;
    case AST_FUNCDEF_NODE: { ALLOC_AST_NODE_DATA(node->as.funcdef, struct ASTFuncDefData); } break;
    case AST_BUILTIN_TYPEREF_NODE: { ALLOC_AST_NODE_DATA(node->as.builtin_typeref, struct ASTBuiltinTyperefData); } break;
    case AST_CUSTOM_TYPEREF_NODE: { ALLOC_AST_NODE_DATA(node->as.custom_typeref, struct ASTCustomTyperefData); } break;
    case AST_ARRAY_TYPEREF_NODE: { ALLOC_AST_NODE_DATA(node->as.array_typeref, struct ASTArrayTyperefData); } break;
    case AST_VARDECL_STMT_NODE: { ALLOC_AST_NODE_DATA(node->as.vardecl_stmt, struct ASTVardeclStmtData); } break;
    case AST_CONDITION_STMT_NODE: { ALLOC_AST_NODE_DATA(node->as.condition_stmt, struct ASTConditionStmtData); } break;
    case AST_WHILE_STMT_NODE: { ALLOC_AST_NODE_DATA(node->as.while_stmt, struct ASTLoopStmtData); } break;
    case AST_DO_WHILE_STMT_NODE: { ALLOC_AST_NODE_DATA(node->as.do_while_stmt, struct ASTLoopStmtData); } break;
    case AST_BREAK_STMT_NODE: { /* DO NOTHING */ } break;
    case AST_CONTINUE_STMT_NODE: {
        /* DO NOTHING */ } break;
    case AST_EXPRESSION_STMT_NODE: { ALLOC_AST_NODE_DATA(node->as.expression_stmt, struct ASTExpressionStmtData); } break;
    case AST_RETURN_STMT_NODE: { ALLOC_AST_NODE_DATA(node->as.return_stmt, struct ASTExpressionStmtData); } break;
    case AST_BINARY_EXPR_NODE: { ALLOC_AST_NODE_DATA(node->as.binary_expr, struct ASTBinaryExprData); } break;
    case AST_UNARY_EXPR_NODE: { ALLOC_AST_NODE_DATA(node->as.unary_expr, struct ASTUnaryExprData); } break;
    case AST_BRACES_EXPR_NODE: { ALLOC_AST_NODE_DATA(node->as.braces_expr, struct ASTBracesExprData); } break;
    case AST_CALL_OR_INDEXER_EXPR_NODE: { ALLOC_AST_NODE_DATA(node->as.call_or_indexer_expr, struct ASTCallOrIndexerExprData); } break;
    case AST_PLACE_EXPR_NODE: { ALLOC_AST_NODE_DATA(node->as.place_expr, struct ASTPlaceExpr); } break;
    case AST_TERNARY_EXPR_NODE: { ALLOC_AST_NODE_DATA(node->as.ternary_expr, struct ASTTernaryExpr); } break;
    case AST_STRING_LITERAL_NODE: { ALLOC_AST_NODE_DATA(node->as.literal, struct ASTLiteralDataExpr); } break;
    case AST_CHAR_LITERAL_NODE: { ALLOC_AST_NODE_DATA(node->as.literal, struct ASTLiteralDataExpr); } break;
    case AST_DEC_LITERAL_NODE: { ALLOC_AST_NODE_DATA(node->as.literal, struct ASTLiteralDataExpr); } break;
    case AST_HEX_LITERAL_NODE: { ALLOC_AST_NODE_DATA(node->as.literal, struct ASTLiteralDataExpr); } break;
    case AST_OCT_LITERAL_NODE: { ALLOC_AST_NODE_DATA(node->as.literal, struct ASTLiteralDataExpr); } break;
    case AST_BITS_LITERAL_NODE: { ALLOC_AST_NODE_DATA(node->as.literal, struct ASTLiteralDataExpr); } break;
    case AST_BOOL_LITERAL_NODE: { ALLOC_AST_NODE_DATA(node->as.literal, struct ASTLiteralDataExpr); } break;
    default: { assert(false && "Unreachable"); }
    };

    return node;
}

void ast_node_free(ASTNode* node) {
    if (node == NULL) {
        return;
    }

    switch (node->kind) {
    case AST_FUNCSIGN_NODE: {
        ast_node_free(node->as.funcsign->id);
        ast_node_free(node->as.funcsign->typeref);
        vector_free(&node->as.funcsign->args);
        free(node->as.funcdef);
    } break;
    case AST_ARGDEF_NODE: {
        ast_node_free(node->as.argdef->id);
        ast_node_free(node->as.argdef->typeref);
        free(node->as.argdef);
    } break;
    case AST_IDENTIFIER_NODE: {
        free(node->as.identifier->value);
        free(node->as.identifier);
    } break;
    case AST_FUNCDEF_NODE: {
        ast_node_free(node->as.funcdef->funcsign);
        vector_free(&node->as.funcdef->stmts);
        free(node->as.funcdef);
    } break;
    case AST_BUILTIN_TYPEREF_NODE: {
        free(node->as.builtin_typeref->value);
        free(node->as.builtin_typeref);
    } break;
    case AST_CUSTOM_TYPEREF_NODE: {
        ast_node_free(node->as.custom_typeref->id);
        free(node->as.custom_typeref);
    } break;
    case AST_ARRAY_TYPEREF_NODE: {
        ast_node_free(node->as.array_typeref->typeref);
        free(node->as.array_typeref);
    } break;
    case AST_VARDECL_STMT_NODE: {
        ast_node_free(node->as.vardecl_stmt->typeref);
        vector_free(&node->as.vardecl_stmt->ids);
        free(node->as.vardecl_stmt);
    } break;
    case AST_CONDITION_STMT_NODE: {
        ast_node_free(node->as.condition_stmt->expr);
        vector_free(&node->as.condition_stmt->then_branch);
        vector_free(&node->as.condition_stmt->else_branch);
        free(node->as.condition_stmt);
    } break;
    case AST_WHILE_STMT_NODE: {
        ast_node_free(node->as.while_stmt->expr);
        vector_free(&node->as.while_stmt->stmts);
        free(node->as.while_stmt);
    } break;
    case AST_DO_WHILE_STMT_NODE: {
        ast_node_free(node->as.do_while_stmt->expr);
        vector_free(&node->as.do_while_stmt->stmts);
        free(node->as.do_while_stmt);
    } break;
    case AST_BREAK_STMT_NODE: { /* DO NOTHING */ } break;
    case AST_CONTINUE_STMT_NODE: { /* DO NOTHING */ } break;
    case AST_EXPRESSION_STMT_NODE: {
        ast_node_free(node->as.expression_stmt->expr);
        free(node->as.expression_stmt);
    } break;
    case AST_RETURN_STMT_NODE: {
        ast_node_free(node->as.return_stmt->expr);
        free(node->as.return_stmt);
    } break;
    case AST_BINARY_EXPR_NODE: {
        free(node->as.binary_expr->op);
        ast_node_free(node->as.binary_expr->lhs);
        ast_node_free(node->as.binary_expr->rhs);
        free(node->as.binary_expr);
    } break;
    case AST_UNARY_EXPR_NODE: {
        free(node->as.unary_expr->op);
        ast_node_free(node->as.unary_expr->rhs);
        free(node->as.unary_expr);
    } break;
    case AST_BRACES_EXPR_NODE: {
        ast_node_free(node->as.braces_expr->expr);
        free(node->as.braces_expr);
    } break;
    case AST_CALL_OR_INDEXER_EXPR_NODE: {
        ast_node_free(node->as.call_or_indexer_expr->callee);
        vector_free(&node->as.call_or_indexer_expr->args);
        free(node->as.call_or_indexer_expr);
    } break;
    case AST_PLACE_EXPR_NODE: {
        ast_node_free(node->as.place_expr->id);
        free(node->as.place_expr);
    } break;
    case AST_TERNARY_EXPR_NODE: {
        ast_node_free(node->as.ternary_expr->expr);
        ast_node_free(node->as.ternary_expr->then_expr);
        ast_node_free(node->as.ternary_expr->else_expr);
        free(node->as.place_expr);
    } break;
    case AST_STRING_LITERAL_NODE:
    case AST_CHAR_LITERAL_NODE:
    case AST_DEC_LITERAL_NODE:
    case AST_HEX_LITERAL_NODE:
    case AST_OCT_LITERAL_NODE:
    case AST_BITS_LITERAL_NODE:
    case AST_BOOL_LITERAL_NODE: {
        free(node->as.literal->value);
        free(node->as.literal);
    } break;
    };

    free(node);
}