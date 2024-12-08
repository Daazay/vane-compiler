#pragma once

#include "vanec/utils/vector.h"
#include "vanec/utils/source_loc.h"
#include "vanec/frontend/ast/ast_node_kind.h"
#include "vanec/frontend/lexer/token.h"

typedef struct ASTNode ASTNode;

struct ASTFuncSignData {
    ASTNode* id;
    Vector args;
    ASTNode* typeref;
};

struct ASTArgDefData {
    ASTNode* id;
    ASTNode* typeref;
};

struct ASTIdentifierData {
    char* value;
};

struct ASTFuncDefData {
    ASTNode* funcsign;
    Vector stmts;
};

struct ASTBuiltinTyperefData {
    char* value;
};

struct ASTCustomTyperefData {
    ASTNode* id;
};

struct ASTArrayTyperefData {
    ASTNode* typeref;
    u64 dimension;
};

struct ASTVardeclStmtData {
    Vector ids;
    ASTNode* typeref;
};

struct ASTConditionStmtData {
    ASTNode* expr;
    Vector then_branch;
    Vector else_branch;
};

struct ASTLoopStmtData {
    ASTNode* expr;
    Vector stmts;
};

struct ASTExpressionStmtData {
    ASTNode* expr;
};

struct ASTBinaryExprData {
    char* op;
    ASTNode* lhs;
    ASTNode* rhs;
};

struct ASTUnaryExprData {
    char* op;
    ASTNode* rhs;
};

struct ASTBracesExprData {
    ASTNode* expr;
};

struct ASTCallOrIndexerExprData {
    ASTNode* callee;
    Vector args;
};

struct ASTPlaceExpr {
    ASTNode* id;
};

struct ASTTernaryExpr {
    ASTNode* expr;
    ASTNode* then_expr;
    ASTNode* else_expr;
};

struct ASTLiteralDataExpr {
    char* value;
};

struct ASTNode {
    ASTNodeKind kind;

    union {
        struct ASTFuncSignData* funcsign;
        struct ASTArgDefData* argdef;
        struct ASTIdentifierData* identifier;

        struct ASTFuncDefData* funcdef;

        struct ASTBuiltinTyperefData* builtin_typeref;
        struct ASTCustomTyperefData* custom_typeref;
        struct ASTArrayTyperefData* array_typeref;

        struct ASTVardeclStmtData* vardecl_stmt;
        struct ASTConditionStmtData* condition_stmt;
        struct ASTLoopStmtData* while_stmt;
        struct ASTLoopStmtData* do_while_stmt;
        struct ASTExpressionStmtData* expression_stmt;

        struct ASTBinaryExprData* binary_expr;
        struct ASTUnaryExprData* unary_expr;
        struct ASTBracesExprData* braces_expr;
        struct ASTCallOrIndexerExprData* call_or_indexer_expr;
        struct ASTPlaceExpr* place_expr;
        struct ASTTernaryExpr* ternary_expr;

        struct ASTLiteralDataExpr* literal;
    } as;

    SourceLoc loc;
};

ASTNode* ast_node_create(const ASTNodeKind kind);

void ast_node_free(ASTNode* node);