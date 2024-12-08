#pragma once

#include "vanec/frontend/ast/ast_node.h"
#include "vanec/frontend/lexer/token.h"
#include "vanec/frontend/lexer/lexer.h"
#include "vanec/frontend/lexer/token_stream.h"

typedef struct {
    TokenStream ts;
    DiagnosticEngine* diag;
} ASTParser;

ASTParser ast_parser_create(Lexer* lexer, DiagnosticEngine* diag);

void ast_parser_free(ASTParser* parser);

bool is_ast_parser_done(const ASTParser* parser);

const Token* ast_parser_expect_next(ASTParser* parser, const bool consume, bool report, const u64 count, const TokenKind expected[]);

void ast_parser_skip_to_the_end_of_a_file(ASTParser* parser);

ASTNode* ast_parser_parse_ast_funcdef_node(ASTParser* parser);

ASTNode* ast_parser_parse_ast_identifier_node(ASTParser* parser);

ASTNode* ast_parser_parse_ast_basic_typeref_node(ASTParser* parser);

ASTNode* ast_parser_parse_ast_typeref_node(ASTParser* parser);

ASTNode* ast_parser_parse_ast_argdef_node(ASTParser* parser);

ASTNode* ast_parser_parse_ast_funcsign_node(ASTParser* parser);

ASTNode* ast_parser_parse_ast_statement_node(ASTParser* parser);

ASTNode* ast_parser_parse_ast_vardecl_stmt_node(ASTParser* parser);

ASTNode* ast_parser_parse_ast_condition_stmt_node(ASTParser* parser);

ASTNode* ast_parser_parse_ast_while_stmt_node(ASTParser* parser);

ASTNode* ast_parser_parse_ast_do_while_stmt_node(ASTParser* parser);

ASTNode* ast_parser_parse_ast_break_stmt_node(ASTParser* parser);

ASTNode* ast_parser_parse_ast_continue_stmt_node(ASTParser* parser);

ASTNode* ast_parser_parse_ast_expression_stmt_node(ASTParser* parser);

ASTNode* ast_parser_parse_ast_literal_expr_node(ASTParser* parser);

ASTNode* ast_parser_parse_ast_place_expr_node(ASTParser* parser);

ASTNode* ast_parser_parse_ast_unary_expr_node(ASTParser* parser);

ASTNode* ast_parser_parse_ast_braces_expr_node(ASTParser* parser);

ASTNode* ast_parser_parse_ast_lhs_expression_node(ASTParser* parser);

ASTNode* ast_parser_parse_ast_call_or_indexer_expr_node(ASTParser* parser, const ASTNode* callee);

ASTNode* ast_parser_parse_ast_binary_expr_node(ASTParser* parser, const ASTNode* lhs, const Precedence precedence);

ASTNode* ast_parser_parse_ast_ternary_expr_node(ASTParser* parser, const ASTNode* expr);

ASTNode* ast_parser_parse_ast_rhs_expression_node(ASTParser* parser, const ASTNode* lhs, const Precedence precedence);

ASTNode* ast_parser_parse_ast_expression_node(ASTParser* parser, const Precedence precedence);