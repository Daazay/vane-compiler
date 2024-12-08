#include "vanec/frontend/ast/ast_parser.h"

#include <assert.h>
#include <stdlib.h>

#include "vanec/utils/string_builder.h"
#include "vanec/utils/string_utils.h"

ASTParser ast_parser_create(Lexer* lexer, DiagnosticEngine* diag) {
    assert(lexer != NULL);

    return (ASTParser) {
        .ts = token_stream_create(lexer),
        .diag = diag,
    };
}

void ast_parser_free(ASTParser* parser) {
    if (parser == NULL) {
        return;
    }

    token_stream_free(&parser->ts);
    parser->diag = NULL;
}

void ast_parser_clear(ASTParser* parser) {
    assert(parser != NULL);

    token_stream_clear(&parser->ts);
}

bool is_ast_parser_done(ASTParser* parser) {
    assert(parser != NULL);

    return token_stream_peek_next(&parser->ts)->kind == TOKEN_END_OF_FILE;
}

static char* get_token_kind_name_for_diagnostic_report(const TokenKind kind) {
    if (is_token_kind_a_punctuator(kind)) {
        return str_format("'%s'", get_token_kind_value(kind));
    }
    return str_format("'%s'", get_token_kind_spelling(kind));
}

const Token* ast_parser_expect_next(ASTParser* parser, const bool consume, bool report, const u64 count, const TokenKind expected[]) {
    assert(parser != NULL);
    assert(count != 0 && expected != NULL);

    const Token* token = token_stream_peek_next(&parser->ts);

    for (u64 i = 0; i < count; ++i) {
        if (token->kind == expected[i]) {
            if (consume) {
                token_stream_consume(&parser->ts);
            }
            return token;
        }
    }

    if (!report || parser->diag == NULL) {
        return NULL;
    }


    char* received_str = get_token_kind_name_for_diagnostic_report(token->kind);

    StringBuilder sb = string_builder_create();

    for (u64 i = 0; i < count; ++i) {
        char* expected_str = get_token_kind_name_for_diagnostic_report(expected[i]);
        string_builder_append_str_right(&sb, expected_str);
        str_free(expected_str);

        if (i + 1 < count) {
            string_builder_append_char_right(&sb, '|');
        }
    }

    char* expected_str = string_builder_get_str(&sb);
    string_builder_free(&sb);

    diagnostic_engine_report(parser->diag, ERR_EXPECT_TOKEN_KIND_OF, token->loc, expected_str, received_str);

    str_free(expected_str);
    str_free(received_str);

    return NULL;
}

#define EXPECT_NEXT(consume, report, ...)   \
ast_parser_expect_next(parser, consume, report, (sizeof((TokenKind[]){__VA_ARGS__}) / sizeof(TokenKind)), (TokenKind[]) {__VA_ARGS__})

void ast_parser_skip_to_the_end_of_a_file(ASTParser* parser) {
    assert(parser != NULL);

    while (token_stream_peek_next(&parser->ts)->kind != TOKEN_END_OF_FILE) {
        token_stream_consume(&parser->ts);
    }
}

ASTNode* ast_parser_parse_ast_funcdef_node(ASTParser* parser) {
    assert(parser != NULL);

    const Token* token = EXPECT_NEXT(true, true, TOKEN_FUNCTION_KEYWORD);
    if (token == NULL) {
        return NULL;
    }

    SourceLoc loc = token->loc;

    ASTNode* funcsign = ast_parser_parse_ast_funcsign_node(parser);
    if (funcsign == NULL) {
        return NULL;
    }

    Vector stmts = vector_create(DEFAULT_VECTOR_CAPACITY, sizeof(ASTNode*), &ast_node_free, true);

    token = token_stream_peek_next(&parser->ts);
    
    while (token->kind != TOKEN_END_OF_FILE && token->kind != TOKEN_END_KEYWORD) {
        ASTNode* stmt = ast_parser_parse_ast_statement_node(parser);

        if (stmt == NULL) {
            ast_node_free(funcsign);
            vector_free(&stmts);
            return NULL;
        }

        vector_push_back(&stmts, &stmt);

        token = token_stream_peek_next(&parser->ts);
    }

    token = EXPECT_NEXT(true, true, TOKEN_END_KEYWORD);
    if (token == NULL) {
        ast_node_free(funcsign);
        vector_free(&stmts);
        return NULL;
    }

    token = EXPECT_NEXT(true, true, TOKEN_FUNCTION_KEYWORD);
    if (token == NULL) {
        ast_node_free(funcsign);
        vector_free(&stmts);
        return NULL;
    }

    loc.end = token->loc.end;

    ASTNode* funcdef = ast_node_create(AST_FUNCDEF_NODE);
    
    funcdef->loc = loc;
    funcdef->as.funcdef->funcsign = funcsign;
    funcdef->as.funcdef->stmts = stmts;

    return funcdef;
}

ASTNode* ast_parser_parse_ast_identifier_node(ASTParser* parser) {
    assert(parser != NULL);

    const Token* token = EXPECT_NEXT(true, true, TOKEN_IDENTIFIER);
    if (token == NULL) { return NULL; }

    ASTNode* id = ast_node_create(AST_IDENTIFIER_NODE);

    id->loc = token->loc;
    id->as.identifier->value = str_dup(token->value);

    return id;
}

ASTNode* ast_parser_parse_ast_basic_typeref_node(ASTParser* parser) {
    assert(parser != NULL);

    const Token* token = EXPECT_NEXT(true, true,
        TOKEN_INT_BUILTIN, TOKEN_UINT_BUILTIN, TOKEN_LONG_BUILTIN,
        TOKEN_ULONG_BUILTIN, TOKEN_BOOL_BUILTIN, TOKEN_BYTE_BUILTIN,
        TOKEN_CHAR_BUILTIN, TOKEN_STRING_BUILTIN, TOKEN_IDENTIFIER
    );
    if (token == NULL) { return NULL; }

    if (is_token_kind_a_builtin(token->kind)) {
        ASTNode* builtin = ast_node_create(AST_BUILTIN_TYPEREF_NODE);

        builtin->loc = token->loc;
        builtin->as.builtin_typeref->value = str_dup(get_token_kind_spelling(token->kind));

        return builtin;
    }

    ASTNode* id = ast_node_create(AST_IDENTIFIER_NODE);

    id->loc = token->loc;
    id->as.identifier->value = str_dup(token->value);

    ASTNode* custom = ast_node_create(AST_CUSTOM_TYPEREF_NODE);

    custom->loc = token->loc;
    custom->as.custom_typeref->id = id;

    return custom;
}

ASTNode* ast_parser_parse_ast_typeref_node(ASTParser* parser) {
    assert(parser != NULL);

    ASTNode* typeref = ast_parser_parse_ast_basic_typeref_node(parser);
    if (typeref == NULL) { return NULL; }

    SourceLoc loc = typeref->loc;

    while (token_stream_peek_next(&parser->ts)->kind == TOKEN_L_BRACE) {
        token_stream_consume(&parser->ts);

        const Token* token = EXPECT_NEXT(false, false, TOKEN_COMMA, TOKEN_R_BRACE);
        if (token == NULL) {
            token_stream_move_back(&parser->ts);
            break;
        }

        u64 dimension = 1;

        while (token->kind != TOKEN_END_OF_FILE && token->kind != TOKEN_R_BRACE) {
            token = EXPECT_NEXT(true, false, TOKEN_COMMA);
            if (token != NULL) {
                ++dimension;
                //token_stream_consume(&parser->ts);
                token = token_stream_peek_next(&parser->ts);
                continue;
            }
            break;
        }

        if (token->kind != TOKEN_R_BRACE) {
            EXPECT_NEXT(false, true, TOKEN_COMMA, TOKEN_R_BRACE);
            ast_node_free(typeref);
            return NULL;
        }
        token_stream_consume(&parser->ts);

        loc.end = token->loc.end;

        ASTNode* array = ast_node_create(AST_ARRAY_TYPEREF_NODE);

        array->loc = loc;
        array->as.array_typeref->typeref = typeref;
        array->as.array_typeref->dimension = dimension;

        typeref = array;
    }

    return typeref;
}

ASTNode* ast_parser_parse_ast_argdef_node(ASTParser* parser) {
    assert(parser != NULL);

    ASTNode* id = ast_parser_parse_ast_identifier_node(parser);
    if (id == NULL) {
        return NULL;
    }

    ASTNode* typeref = NULL;
    if (EXPECT_NEXT(true, false, TOKEN_AS_KEYWORD) != NULL) {
        typeref = ast_parser_parse_ast_typeref_node(parser);
        if (typeref == NULL) {
            ast_node_free(id);
            return NULL;
        }
    }

    SourceLoc loc = id->loc;
    if (typeref != NULL) {
        loc.end = typeref->loc.end;
    }

    ASTNode* argdef = ast_node_create(AST_ARGDEF_NODE);

    argdef->loc = loc;
    argdef->as.argdef->id = id;
    argdef->as.argdef->typeref = typeref;

    return argdef;
}

ASTNode* ast_parser_parse_ast_funcsign_node(ASTParser* parser) {
    assert(parser != NULL);

    ASTNode* id = ast_parser_parse_ast_identifier_node(parser);
    if (id == NULL) {
        return NULL;
    }

    if (EXPECT_NEXT(true, true, TOKEN_L_BRACE) == NULL) {
        ast_node_free(id);
        return NULL;
    }

    Vector args = vector_create(DEFAULT_VECTOR_CAPACITY, sizeof(ASTNode*), &ast_node_free, true);

    const Token* token = token_stream_peek_next(&parser->ts);

    bool is_first = true;
    while (token->kind != TOKEN_END_OF_FILE && token->kind != TOKEN_R_BRACE) {
        if (!is_first) {
            if ((token = EXPECT_NEXT(true, false, TOKEN_COMMA)) == NULL) {
                break;
            }
        }

        ASTNode* argdef = ast_parser_parse_ast_argdef_node(parser);

        if (argdef == NULL) {
            ast_node_free(id);
            vector_free(&args);
            return NULL;
        }

        vector_push_back(&args, &argdef);

        token = token_stream_peek_next(&parser->ts);
        is_first = false;
    }

    if ((token = EXPECT_NEXT(true, true, TOKEN_R_BRACE)) == NULL) {
        ast_node_free(id);
        vector_free(&args);
        return NULL;
    }

    SourceLoc loc = id->loc;
    loc.end = token->loc.end;

    ASTNode* typeref = NULL;
    if (EXPECT_NEXT(true, false, TOKEN_AS_KEYWORD)) {
        typeref = ast_parser_parse_ast_typeref_node(parser);

        if (typeref == NULL) {
            ast_node_free(id);
            vector_free(&args);
            return NULL;
        }
    }

    if (typeref != NULL) {
        loc.end = typeref->loc.end;
    }

    ASTNode* funcsign = ast_node_create(AST_FUNCSIGN_NODE);

    funcsign->loc = loc;
    funcsign->as.funcsign->id = id;
    funcsign->as.funcsign->args = args;
    funcsign->as.funcsign->typeref = typeref;

    return funcsign;
}

ASTNode* ast_parser_parse_ast_statement_node(ASTParser* parser) {
    assert(parser != NULL);
    
    const Token* token = token_stream_peek_next(&parser->ts);
    
    switch (token->kind) {
        /* STATEMENTS */
    case TOKEN_DIM_KEYWORD:         return ast_parser_parse_ast_vardecl_stmt_node(parser);
    case TOKEN_IF_KEYWORD:          return ast_parser_parse_ast_condition_stmt_node(parser);
    case TOKEN_WHILE_KEYWORD:       return ast_parser_parse_ast_while_stmt_node(parser);
    case TOKEN_DO_KEYWORD:          return ast_parser_parse_ast_do_while_stmt_node(parser);
    case TOKEN_BREAK_KEYWORD:       return ast_parser_parse_ast_break_stmt_node(parser);
    case TOKEN_CONTINUE_KEYWORD:    return ast_parser_parse_ast_continue_stmt_node(parser);
        /* EXPRESSION STATEMENT */
    case TOKEN_PLUS:                return ast_parser_parse_ast_expression_stmt_node(parser);
    case TOKEN_PLUS_PLUS:           return ast_parser_parse_ast_expression_stmt_node(parser);
    case TOKEN_MINUS:               return ast_parser_parse_ast_expression_stmt_node(parser);
    case TOKEN_MINUS_MINUS:         return ast_parser_parse_ast_expression_stmt_node(parser);
    case TOKEN_EXCLAIM:             return ast_parser_parse_ast_expression_stmt_node(parser);
    case TOKEN_TILDE:               return ast_parser_parse_ast_expression_stmt_node(parser);
    case TOKEN_L_BRACE:             return ast_parser_parse_ast_expression_stmt_node(parser);
    case TOKEN_STRING_LITERAL:      return ast_parser_parse_ast_expression_stmt_node(parser);
    case TOKEN_CHAR_LITERAL:        return ast_parser_parse_ast_expression_stmt_node(parser);
    case TOKEN_DEC_LITERAL:         return ast_parser_parse_ast_expression_stmt_node(parser);
    case TOKEN_HEX_LITERAL:         return ast_parser_parse_ast_expression_stmt_node(parser);
    case TOKEN_OCT_LITERAL:         return ast_parser_parse_ast_expression_stmt_node(parser);
    case TOKEN_BITS_LITERAL:        return ast_parser_parse_ast_expression_stmt_node(parser);
    case TOKEN_BOOL_LITERAL:        return ast_parser_parse_ast_expression_stmt_node(parser);
    case TOKEN_IDENTIFIER:          return ast_parser_parse_ast_expression_stmt_node(parser);
        /* ------------- */
    default: {
        EXPECT_NEXT(false, true, 
            /* STATEMENT */
            TOKEN_DIM_KEYWORD, TOKEN_IF_KEYWORD, TOKEN_WHILE_KEYWORD, 
            TOKEN_DO_KEYWORD, TOKEN_BREAK_KEYWORD, TOKEN_CONTINUE_KEYWORD,
            /* EXPRESSION */
            TOKEN_PLUS, TOKEN_PLUS_PLUS, TOKEN_MINUS, TOKEN_MINUS_MINUS,
            TOKEN_EXCLAIM, TOKEN_TILDE, TOKEN_L_BRACE, TOKEN_STRING_LITERAL,
            /* LITERAL */
            TOKEN_CHAR_LITERAL, TOKEN_DEC_LITERAL, TOKEN_HEX_LITERAL,
            TOKEN_OCT_LITERAL, TOKEN_BITS_LITERAL, TOKEN_BOOL_LITERAL,
            /* PLACE EXPRESSION */
            TOKEN_IDENTIFIER
            );
        return NULL;
    }
    };
}

ASTNode* ast_parser_parse_ast_vardecl_stmt_node(ASTParser* parser) {
    assert(parser != NULL);

    const Token* token = EXPECT_NEXT(true, true, TOKEN_DIM_KEYWORD);
    if (token == NULL) {
        return NULL;
    }

    SourceLoc loc = token->loc;

    Vector ids = vector_create(DEFAULT_VECTOR_CAPACITY, sizeof(ASTNode*), &ast_node_free, true);

    bool is_first = true;
    do {
        if (!is_first) {
            if ((token = EXPECT_NEXT(true, false, TOKEN_COMMA)) == NULL) {
                break;
            }
        }

        ASTNode* id = ast_parser_parse_ast_identifier_node(parser);
        if (id == NULL) {
            vector_free(&ids);
            return NULL;
        }

        vector_push_back(&ids, &id);

        token = token_stream_peek_next(&parser->ts);
        is_first = false;
    } while (token->kind != TOKEN_END_OF_FILE && token->kind != TOKEN_AS_KEYWORD);

    if ((token = EXPECT_NEXT(true, true, TOKEN_AS_KEYWORD)) == NULL) {
        vector_free(&ids);
        return NULL;
    }

    ASTNode* typeref = ast_parser_parse_ast_typeref_node(parser);
    if (typeref == NULL) {
        vector_free(&ids);
        return NULL;
    }

    loc.end = typeref->loc.end;

    ASTNode* vardecl = ast_node_create(AST_VARDECL_STMT_NODE);

    vardecl->loc = loc;
    vardecl->as.vardecl_stmt->ids = ids;
    vardecl->as.vardecl_stmt->typeref = typeref;

    return vardecl;
}

ASTNode* ast_parser_parse_ast_condition_stmt_node(ASTParser* parser) {
    assert(parser != NULL);

    const Token* token = EXPECT_NEXT(true, true, TOKEN_IF_KEYWORD);
    if (token == NULL) {
        return NULL;
    }

    SourceLoc loc = token->loc;

    ASTNode* expr = ast_parser_parse_ast_expression_node(parser, PREC_NONE);
    if (expr == NULL) {
        return NULL;
    }

    if ((token = EXPECT_NEXT(true, true, TOKEN_THEN_KEYWORD)) == NULL) {
        ast_node_free(expr);
        return NULL;
    }

    Vector then_branch = vector_create(DEFAULT_VECTOR_CAPACITY, sizeof(ASTNode*), &ast_node_free, true);

    token = token_stream_peek_next(&parser->ts);
    while (token->kind != TOKEN_END_OF_FILE && token->kind != TOKEN_END_KEYWORD && token->kind != TOKEN_ELSE_KEYWORD) {
        ASTNode* stmt = ast_parser_parse_ast_statement_node(parser);

        if (stmt == NULL) {
            ast_node_free(expr);
            vector_free(&then_branch);
            return NULL;
        }

        vector_push_back(&then_branch, &stmt);

        token = token_stream_peek_next(&parser->ts);
    }

    Vector else_branch = vector_create(DEFAULT_VECTOR_CAPACITY, sizeof(ASTNode*), &ast_node_free, true);

    if (token->kind == TOKEN_ELSE_KEYWORD) {
        token_stream_consume(&parser->ts);

        token = token_stream_peek_next(&parser->ts);
        while (token->kind != TOKEN_END_OF_FILE && token->kind != TOKEN_END_KEYWORD) {
            ASTNode* stmt = ast_parser_parse_ast_statement_node(parser);

            if (stmt == NULL) {
                ast_node_free(expr);
                vector_free(&then_branch);
                vector_free(&else_branch);
                return NULL;
            }

            vector_push_back(&else_branch, &stmt);

            token = token_stream_peek_next(&parser->ts);
        }
    }

    if (EXPECT_NEXT(true, true, TOKEN_END_KEYWORD) == NULL) {
        ast_node_free(expr);
        vector_free(&then_branch);
        vector_free(&else_branch);
        return NULL;
    }

    loc.end = token_stream_peek_next(&parser->ts)->loc.end;

    if (EXPECT_NEXT(true, true, TOKEN_IF_KEYWORD) == NULL) {
        ast_node_free(expr);
        vector_free(&then_branch);
        vector_free(&else_branch);
        return NULL;
    }

    ASTNode* condition = ast_node_create(AST_CONDITION_STMT_NODE);

    condition->loc = loc;
    condition->as.condition_stmt->expr = expr;
    condition->as.condition_stmt->then_branch = then_branch;
    condition->as.condition_stmt->else_branch = else_branch;

    return condition;
}

ASTNode* ast_parser_parse_ast_while_stmt_node(ASTParser* parser) {
    assert(parser != NULL);

    const Token* token = EXPECT_NEXT(true, true, TOKEN_WHILE_KEYWORD);
    if (token == NULL) {
        return NULL;
    }

    SourceLoc loc = token->loc;

    ASTNode* expr = ast_parser_parse_ast_expression_node(parser, PREC_NONE);
    if (expr == NULL) {
        return NULL;
    }

    Vector stmts = vector_create(DEFAULT_VECTOR_CAPACITY, sizeof(ASTNode*), &ast_node_free, true);

    token = token_stream_peek_next(&parser->ts);
  
    while (token->kind != TOKEN_END_OF_FILE && token->kind != TOKEN_WEND_KEYWORD) {
        ASTNode* stmt = ast_parser_parse_ast_statement_node(parser);

        if (stmt == NULL) {
            ast_node_free(expr);
            vector_free(&stmts);
            return NULL;
        }

        vector_push_back(&stmts, &stmt);

        token = token_stream_peek_next(&parser->ts);
    }

    loc.end = token_stream_peek_next(&parser->ts)->loc.end;

    if (EXPECT_NEXT(true, true, TOKEN_WEND_KEYWORD) == NULL) {
        ast_node_free(expr);
        vector_free(&stmts);
        return NULL;
    }

    ASTNode* while_loop = ast_node_create(AST_WHILE_STMT_NODE);

    while_loop->loc = loc;
    while_loop->as.while_stmt->expr = expr;
    while_loop->as.while_stmt->stmts = stmts;

    return while_loop;
}

ASTNode* ast_parser_parse_ast_do_while_stmt_node(ASTParser* parser) {
    assert(parser != NULL);

    const Token* token = EXPECT_NEXT(true, true, TOKEN_DO_KEYWORD);
    if (token == NULL) {
        return NULL;
    }

    SourceLoc loc = token->loc;

    Vector stmts = vector_create(DEFAULT_VECTOR_CAPACITY, sizeof(ASTNode*), &ast_node_free, true);

    token = token_stream_peek_next(&parser->ts);

    while (token->kind != TOKEN_END_OF_FILE && token->kind != TOKEN_LOOP_KEYWORD) {
        ASTNode* stmt = ast_parser_parse_ast_statement_node(parser);

        if (stmt == NULL) {
            vector_free(&stmts);
            return NULL;
        }

        vector_push_back(&stmts, &stmt);

        token = token_stream_peek_next(&parser->ts);
    }

    if ((token = EXPECT_NEXT(true, true, TOKEN_LOOP_KEYWORD)) == NULL) {
        vector_free(&stmts);
        return NULL;
    }
    if ((token = EXPECT_NEXT(true, true, TOKEN_WHILE_KEYWORD, TOKEN_UNTIL_KEYWORD)) == NULL) {
        vector_free(&stmts);
        return NULL;
    }

    ASTNode* expr = ast_parser_parse_ast_expression_node(parser, PREC_NONE);

    if (expr == NULL) {
        vector_free(&stmts);
        return NULL;
    }

    loc.end = expr->loc.end;

    ASTNode* do_while = ast_node_create(AST_DO_WHILE_STMT_NODE);
    
    do_while->loc = loc;
    do_while->as.do_while_stmt->expr = expr;
    do_while->as.do_while_stmt->stmts = stmts;

    return do_while;
}

ASTNode* ast_parser_parse_ast_break_stmt_node(ASTParser* parser) {
    assert(parser != NULL);

    const Token* token = EXPECT_NEXT(true, true, TOKEN_BREAK_KEYWORD);
    if (token == NULL) {
        return NULL;
    }

    ASTNode* stmt = ast_node_create(AST_BREAK_STMT_NODE);

    stmt->loc = token->loc;

    return stmt;
}

ASTNode* ast_parser_parse_ast_continue_stmt_node(ASTParser* parser) {
    assert(parser != NULL);

    const Token* token = EXPECT_NEXT(true, true, TOKEN_CONTINUE_KEYWORD);
    if (token == NULL) {
        return NULL;
    }

    ASTNode* stmt = ast_node_create(AST_CONTINUE_STMT_NODE);

    stmt->loc = token->loc;

    return stmt;
}

ASTNode* ast_parser_parse_ast_literal_expr_node(ASTParser* parser) {
    assert(parser != NULL);

    const Token* token = EXPECT_NEXT(true, true,
        TOKEN_STRING_LITERAL, TOKEN_CHAR_LITERAL, TOKEN_DEC_LITERAL, TOKEN_HEX_LITERAL,
        TOKEN_OCT_LITERAL, TOKEN_BITS_LITERAL, TOKEN_BOOL_LITERAL
    );
    if (token == NULL) {
        return NULL;
    }

    ASTNodeKind kind = AST_UNKNOWN_NODE;

    switch (token->kind) {
    case TOKEN_STRING_LITERAL:  { kind = AST_STRING_LITERAL_NODE; } break;
    case TOKEN_CHAR_LITERAL:    { kind = AST_CHAR_LITERAL_NODE; } break;
    case TOKEN_DEC_LITERAL:     { kind = AST_DEC_LITERAL_NODE; } break;
    case TOKEN_HEX_LITERAL:     { kind = AST_HEX_LITERAL_NODE; } break;
    case TOKEN_OCT_LITERAL:     { kind = AST_OCT_LITERAL_NODE; } break;
    case TOKEN_BITS_LITERAL:    { kind = AST_BITS_LITERAL_NODE; } break;
    case TOKEN_BOOL_LITERAL:    { kind = AST_BOOL_LITERAL_NODE; } break;
    default: {
        assert(false && "Unreachable");
    }
    };

    ASTNode* literal = ast_node_create(kind);

    literal->loc = token->loc;
    literal->as.literal->value = str_dup(token->value);

    return literal;
}

ASTNode* ast_parser_parse_ast_place_expr_node(ASTParser* parser) {
    assert(parser != NULL);

    ASTNode* id = ast_parser_parse_ast_identifier_node(parser);
    if (id == NULL) {
        return NULL;
    }

    ASTNode* place = ast_node_create(AST_PLACE_EXPR_NODE);

    place->loc = id->loc;
    place->as.place_expr->id = id;

    return place;
}

ASTNode* ast_parser_parse_ast_unary_expr_node(ASTParser* parser) {
    assert(parser != NULL);

    const Token* token = EXPECT_NEXT(true, true, 
        TOKEN_PLUS, TOKEN_PLUS_PLUS, TOKEN_MINUS,
        TOKEN_MINUS_MINUS, TOKEN_TILDE, TOKEN_EXCLAIM
    );
    if (token == NULL) {
        return NULL;
    }

    SourceLoc loc = token->loc;

    char* op = str_dup(get_token_kind_value(token->kind));

    ASTNode* rhs = ast_parser_parse_ast_expression_node(parser, PREC_NONE);

    if (rhs == NULL) {
        str_free(op);
        return NULL;
    }

    loc.end = rhs->loc.end;

    ASTNode* unary = ast_node_create(AST_UNARY_EXPR_NODE);

    unary->loc = loc;
    unary->as.unary_expr->op = op;
    unary->as.unary_expr->rhs = rhs;

    return unary;
}

ASTNode* ast_parser_parse_ast_braces_expr_node(ASTParser* parser) {
    assert(parser != NULL);

    const Token* token = EXPECT_NEXT(true, true, TOKEN_L_BRACE);
    if (token == NULL) {
        return NULL;
    }

    SourceLoc loc = token->loc;

    ASTNode* expr = ast_parser_parse_ast_expression_node(parser, PREC_NONE);

    if (expr == NULL) {
        return NULL;
    }

    if ((token = EXPECT_NEXT(true, true, TOKEN_R_BRACE)) == NULL) {
        ast_node_free(expr);
        return NULL;
    }

    loc.end = token->loc.end;

    ASTNode* braces = ast_node_create(AST_BRACES_EXPR_NODE);

    braces->loc = loc;
    braces->as.braces_expr->expr = expr;

    return braces;
}

ASTNode* ast_parser_parse_ast_lhs_expression_node(ASTParser* parser) {
    assert(parser != NULL);

    const Token* token = token_stream_peek_next(&parser->ts);

    if (is_token_kind_a_literal(token->kind)) { return ast_parser_parse_ast_literal_expr_node(parser); }
    else if (is_token_kind_an_unary_op(token->kind)) { return ast_parser_parse_ast_unary_expr_node(parser); }
    else if (token->kind == TOKEN_IDENTIFIER) { return ast_parser_parse_ast_place_expr_node(parser); }
    else if (token->kind == TOKEN_L_BRACE) { return ast_parser_parse_ast_braces_expr_node(parser); }

    // ELSE 

    EXPECT_NEXT(false, true,
        TOKEN_STRING_LITERAL, TOKEN_CHAR_LITERAL, TOKEN_DEC_LITERAL, TOKEN_HEX_LITERAL,
        TOKEN_OCT_LITERAL, TOKEN_BITS_LITERAL, TOKEN_BOOL_LITERAL, TOKEN_IDENTIFIER,
        TOKEN_PLUS, TOKEN_PLUS_PLUS, TOKEN_MINUS, TOKEN_MINUS_MINUS, 
        TOKEN_TILDE, TOKEN_EXCLAIM, TOKEN_L_BRACE
    );
    return NULL;
}

ASTNode* ast_parser_parse_ast_call_or_indexer_expr_node(ASTParser* parser, ASTNode* callee) {
    assert(parser != NULL && callee != NULL);

    const Token* token = EXPECT_NEXT(true, true, TOKEN_L_BRACE);
    if (token == NULL) {
        ast_node_free(callee);
        return NULL;
    }

    token = token_stream_peek_next(&parser->ts);

    Vector args = vector_create(DEFAULT_VECTOR_CAPACITY, sizeof(ASTNode*), &ast_node_free, true);

    bool is_first = true;
    while (token->kind != TOKEN_END_OF_FILE && token->kind != TOKEN_R_BRACE) {
        if (!is_first) {
            if ((token = EXPECT_NEXT(true, false, TOKEN_COMMA)) == NULL) {
                break;
            }
        }
        
        ASTNode* arg = ast_parser_parse_ast_expression_node(parser, PREC_COMMA);
        
        if (arg == NULL) {
            ast_node_free(callee);
            vector_free(&args);
            return NULL;
        }

        vector_push_back(&args, &arg);
        
        token = token_stream_peek_next(&parser->ts);
        is_first = false;
    }

    if ((token = EXPECT_NEXT(true, true, TOKEN_R_BRACE)) == NULL) {
        ast_node_free(callee);
        vector_free(&args);
        return NULL;
    }

    ASTNode* call_or_indexer = ast_node_create(AST_CALL_OR_INDEXER_EXPR_NODE);

    call_or_indexer->loc = callee->loc;
    call_or_indexer->loc.end = token->loc.end;
    call_or_indexer->as.call_or_indexer_expr->callee = callee;
    call_or_indexer->as.call_or_indexer_expr->args = args;

    return call_or_indexer;
}

ASTNode* ast_parser_parse_ast_binary_expr_node(ASTParser* parser, ASTNode* lhs, const Precedence precedence) {
    assert(parser != NULL && lhs != NULL);

    const Token* token = EXPECT_NEXT(true, true, 
        TOKEN_PLUS, TOKEN_PLUS_EQUAL, TOKEN_MINUS, TOKEN_MINUS_EQUAL, 
        TOKEN_STAR, TOKEN_STAR_EQUAL, TOKEN_SLASH, TOKEN_SLASH_EQUAL, 
        TOKEN_PERCENT, TOKEN_PERCENT_EQUAL, TOKEN_GREATER, TOKEN_GREATER_EQUAL, 
        TOKEN_GREATER_GREATER, TOKEN_GREATER_GREATER_EQUAL, TOKEN_LESS, TOKEN_LESS_EQUAL, TOKEN_LESS_LESS, 
        TOKEN_LESS_LESS_EQUAL, TOKEN_CARET, TOKEN_CARET_EQUAL, TOKEN_AMP, 
        TOKEN_AMP_AMP, TOKEN_AMP_EQUAL, TOKEN_PIPE, TOKEN_PIPE_PIPE, 
        TOKEN_PIPE_EQUAL, TOKEN_EXCLAIM_EQUAL, TOKEN_EQUAL, TOKEN_EQUAL_EQUAL
    );
    if (token == NULL) {
        ast_node_free(lhs);
        return NULL;
    }

    SourceLoc loc = lhs->loc;

    char* op = str_dup(get_token_kind_value(token->kind));

    const Associativity assoc = get_associativity(precedence);

    ASTNode* rhs = NULL;
    
    if (assoc == ASSOC_LEFT_TO_RIGHT) { rhs = ast_parser_parse_ast_expression_node(parser, precedence); }
    else { rhs = ast_parser_parse_ast_expression_node(parser, precedence - 1); }

    if (rhs == NULL) {
        str_free(op);
        ast_node_free(lhs);
        return NULL;
    }

    loc.end = rhs->loc.end;

    ASTNode* binary = ast_node_create(AST_BINARY_EXPR_NODE);

    binary->loc = loc;
    binary->as.binary_expr->op = op;
    binary->as.binary_expr->lhs = lhs;
    binary->as.binary_expr->rhs = rhs;

    return binary;
}

ASTNode* ast_parser_parse_ast_ternary_expr_node(ASTParser* parser, ASTNode* expr) {
    assert(parser != NULL && expr != NULL);

    const Token* token = EXPECT_NEXT(true, true, TOKEN_QUESTION);
    if (token == NULL) {
        ast_node_free(expr);
        return NULL;
    }

    SourceLoc loc = expr->loc;

    ASTNode* then_expr = ast_parser_parse_ast_expression_node(parser, PREC_NONE);
    if (then_expr == NULL) {
        ast_node_free(expr);
        return NULL;
    }

    if ((token = EXPECT_NEXT(true, true, TOKEN_COLON)) == NULL) {
        ast_node_free(expr);
        ast_node_free(then_expr);
        return NULL;
    }

    ASTNode* else_expr = ast_parser_parse_ast_expression_node(parser, PREC_NONE);
    if (else_expr == NULL) {
        ast_node_free(then_expr);
        ast_node_free(expr);
        return NULL;
    }

    loc.end = else_expr->loc.end;

    ASTNode* ternary = ast_node_create(AST_TERNARY_EXPR_NODE);

    ternary->loc = loc;
    ternary->loc.end = else_expr->loc.end;
    ternary->as.ternary_expr->expr = expr;
    ternary->as.ternary_expr->then_expr = then_expr;
    ternary->as.ternary_expr->else_expr = else_expr;

    return ternary;
}

ASTNode* ast_parser_parse_ast_rhs_expression_node(ASTParser* parser, ASTNode* lhs, const Precedence precedence) {
    assert(parser != NULL && lhs != NULL);

    const Token* token = token_stream_peek_next(&parser->ts);

    if (is_token_kind_a_binary_op(token->kind)) { return ast_parser_parse_ast_binary_expr_node(parser, lhs, precedence); }
    else if (token->kind == TOKEN_L_BRACE) { return ast_parser_parse_ast_call_or_indexer_expr_node(parser, lhs); }
    else if (token->kind == TOKEN_QUESTION) { return ast_parser_parse_ast_ternary_expr_node(parser, lhs); }
   
    // ELSE

    EXPECT_NEXT(false, true,
        TOKEN_L_BRACE, TOKEN_IDENTIFIER, TOKEN_PLUS, TOKEN_PLUS_PLUS, 
        TOKEN_MINUS, TOKEN_MINUS_MINUS, TOKEN_TILDE, TOKEN_EXCLAIM,
        TOKEN_L_BRACE
    );
    return NULL;
}

ASTNode* ast_parser_parse_ast_expression_node(ASTParser* parser, const Precedence precedence) {
    assert(parser != NULL);

    ASTNode* expr = ast_parser_parse_ast_lhs_expression_node(parser);

    if (expr == NULL) {
        return NULL;
    }

    const Token* tok = token_stream_peek_next(&parser->ts);
    while (tok->kind != TOKEN_END_OF_FILE) {
        const Precedence new_precedence = get_precedence(tok->kind);
        if (new_precedence <= precedence) {
            break;
        }

        expr = ast_parser_parse_ast_rhs_expression_node(parser, expr, new_precedence);
        if (expr == NULL) {
            return NULL;
        }

        tok = token_stream_peek_next(&parser->ts);
    }

    return expr;
}

ASTNode* ast_parser_parse_ast_expression_stmt_node(ASTParser* parser) {
    assert(parser != NULL);

    ASTNode* expr = ast_parser_parse_ast_expression_node(parser, PREC_NONE);
    if (expr == NULL) {
        return NULL;
    }

    const Token* token = EXPECT_NEXT(true, true, TOKEN_SEMICOLON);
    if (token == NULL) {
        ast_node_free(expr);
        return NULL;
    }

    ASTNode* stmt = ast_node_create(AST_EXPRESSION_STMT_NODE);

    stmt->loc = expr->loc;
    stmt->loc.end = token->loc.end;
    stmt->as.expression_stmt->expr = expr;

    return stmt;
}