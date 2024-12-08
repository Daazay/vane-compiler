#include "vanec/frontend/ast/ast_node_utils.h"

#include <assert.h>

#include "vanec/utils/defines.h"
#include "vanec/utils/string_utils.h"
#include "vanec/utils/string_builder.h"

// COLORS
#define COLOR_ORANGERED         "orangered"
#define COLOR_TOMATO            "tomato"
#define COLOR_DODGERBLUE        "dodgerblue"
#define COLOR_DEEPSKYBLUE       "deepskyblue"
#define COLOR_GREENYELLOW       "greenyellow"
#define COLOR_CORAL             "coral"
#define COLOR_CORAL1            "coral1"
#define COLOR_CORAL2            "coral2"
#define COLOR_CORAL3            "coral3"
#define COLOR_DARKORCHID        "darkorchid"
#define COLOR_DARKORCHID1       "darkorchid1"
#define COLOR_DARKORCHID2       "darkorchid2"
#define COLOR_DARKORCHID3       "darkorchid3"
#define COLOR_DARKORCHID4       "darkorchid4"
#define COLOR_SPRINGGREEN       "springgreen"
#define COLOR_SPRINGGREEN1      "springgreen1"
#define COLOR_SPRINGGREEN2      "springgreen2"
#define COLOR_SPRINGGREEN3      "springgreen3"
#define COLOR_SPRINGGREEN4      "springgreen4"
#define COLOR_SEAGRING          "seagreen"
#define COLOR_SEAGRING1         "seagreen1"
#define COLOR_SEAGRING2         "seagreen2"
#define COLOR_SEAGRING3         "seagreen3"
#define COLOR_SEAGRING4         "seagreen4"
#define COLOR_SKYBLUE           "skyblue"
#define COLOR_SKYBLUE1          "skyblue1"
#define COLOR_SKYBLUE2          "skyblue2"
#define COLOR_SKYBLUE3          "skyblue3"
#define COLOR_SKYBLUE4          "skyblue4"
#define COLOR_TAN1              "tan1"
#define COLOR_PALEVIOLETRED     "palevioletred"
#define COLOR_PALEVIOLETRED1    "palevioletred1"
#define COLOR_PALEVIOLETRED2    "palevioletred2"
#define COLOR_PALEVIOLETRED3    "palevioletred3"
#define COLOR_PALEVIOLETRED4    "palevioletred4"
#define COLOR_LIGHTGREEN        "lightgreen"
#define COLOR_LIGHTCORAL        "lightcoral"
#define COLOR_THISTLE           "thistle"

// COLOR SCHEME
/* MODULE */
#define COLOR_SCHEME_MODULE_COLOR                           COLOR_ORANGERED
#define COLOR_SCHEME_MODULE_ITEMS_COLOR                     COLOR_TOMATO
/* FUNCSIGN */
#define COLOR_SCHEME_FUNCSIGN_COLOR                         COLOR_DEEPSKYBLUE
#define COLOR_SCHEME_FUNCSIGN_ARGS_COLOR                    COLOR_DARKORCHID
/* ARGDEF */
#define COLOR_SCHEME_ARGDEF_COLOR                           COLOR_DARKORCHID2
/* IDENTIFIER */
#define COLOR_SCHEME_IDENTIFIER_COLOR                       COLOR_SEAGRING2
#define COLOR_SCHEME_IDENTIFIER_VALUE_COLOR                 COLOR_SEAGRING3
/* FUNCDEF */
#define COLOR_SCHEME_FUNCDEF_COLOR                          COLOR_DODGERBLUE
#define COLOR_SCHEME_FUNCDEF_STATEMENTS_COLOR               COLOR_SKYBLUE2
/* TYPEREF */
#define COLOR_SCHEME_TYPEREF_COLOR                          COLOR_CORAL
/* BUILTIN TYPEREF */
#define COLOR_SCHEME_BUILTIN_TYPEREF_COLOR                  COLOR_CORAL1
#define COLOR_SCHEME_BUILTIN_TYPEREF_VALUE_COLOR            COLOR_CORAL1
/* CUSTOM TYPEREF */
#define COLOR_SCHEME_CUSTOM_TYPEREF_COLOR                   COLOR_CORAL2
/* ARRAY TYPEREF */
#define COLOR_SCHEME_ARRAY_TYPEREF_COLOR                    COLOR_CORAL3
#define COLOR_SCHEME_ARRAY_TYPEREF_DIMENSION_COLOR          COLOR_CORAL3
#define COLOR_SCHEME_ARRAY_TYPEREF_DIMENSION_VALUE_COLOR    COLOR_CORAL3
/* VARDECL STATEMENT */
#define COLOR_SCHEME_VARDECL_STMT_COLOR                     COLOR_SKYBLUE1
#define COLOR_SCHEME_VARDECL_STMT_IDS_COLOR                 COLOR_SEAGRING1
/* CONDITION STATEMENT */
#define COLOR_SCHEME_CONDITION_STMT_COLOR                   COLOR_SKYBLUE1
#define COLOR_SCHEME_CONDITION_STMT_EXPR_COLOR              COLOR_PALEVIOLETRED
#define COLOR_SCHEME_CONDITION_STMT_THEN_COLOR              COLOR_LIGHTGREEN
#define COLOR_SCHEME_CONDITION_STMT_ELSE_COLOR              COLOR_LIGHTCORAL
/* WHILE STATEMENT */
#define COLOR_SCHEME_WHILE_STMT_COLOR                       COLOR_SKYBLUE1
#define COLOR_SCHEME_WHILE_STMT_EXPR_COLOR                  COLOR_PALEVIOLETRED
#define COLOR_SCHEME_WHILE_STMT_STATEMENTS_COLOR            COLOR_SKYBLUE2
/* DO WHILE STATEMENT */
#define COLOR_SCHEME_DO_WHILE_STMT_COLOR                    COLOR_SKYBLUE2
#define COLOR_SCHEME_DO_WHILE_STMT_EXPR_COLOR               COLOR_PALEVIOLETRED
#define COLOR_SCHEME_DO_WHILE_STMT_STATEMENTS_COLOR         COLOR_SKYBLUE2
/* BREAK STATEMENT */
#define COLOR_SCHEME_BREAK_STMT_COLOR                       COLOR_SKYBLUE1
/* CONTINUE STATEMENT */
#define COLOR_SCHEME_CONTINUE_STMT_COLOR                    COLOR_SKYBLUE1
/* EXPRESSION STATEMENT */
#define COLOR_SCHEME_EXPRESSION_STMT_COLOR                  COLOR_SKYBLUE1
/* BINARY EXPR */
#define COLOR_SCHEME_BINARY_EXPR_COLOR                      COLOR_THISTLE
/* UNARY EXPR */
#define COLOR_SCHEME_UNARY_EXPR_COLOR                       COLOR_THISTLE
/* BRACES EXPR */
#define COLOR_SCHEME_BRACES_EXPR_COLOR                      COLOR_THISTLE
/* CALL OR INDEXER EXPR */
#define COLOR_SCHEME_CALL_OR_INDEXER_EXPR_COLOR             COLOR_THISTLE
#define COLOR_SCHEME_CALL_OR_INDEXER_EXPR_CALLEE_COLOR      COLOR_THISTLE
#define COLOR_SCHEME_CALL_OR_INDEXER_EXPR_ARGS_COLOR        COLOR_THISTLE
/* PLACE EXPR */
#define COLOR_SCHEME_PLACE_EXPR_COLOR                       COLOR_THISTLE
/* TERNARY EXPR */
#define COLOR_SCHEME_TERNARY_EXPR_COLOR                     COLOR_THISTLE
#define COLOR_SCHEME_TERNARY_EXPR_EXPR_COLOR                COLOR_THISTLE
#define COLOR_SCHEME_TERNARY_EXPR_THEN_COLOR                COLOR_THISTLE
#define COLOR_SCHEME_TERNARY_EXPR_ELSE_COLOR                COLOR_THISTLE
/* LITERALS */
#define COLOR_SCHEME_STRING_LITERAL_COLOR                   COLOR_TAN1
#define COLOR_SCHEME_CHAR_LITERAL_COLOR                     COLOR_TAN1
#define COLOR_SCHEME_DEC_LITERAL_COLOR                      COLOR_TAN1
#define COLOR_SCHEME_HEX_LITERAL_COLOR                      COLOR_TAN1
#define COLOR_SCHEME_OCT_LITERAL_COLOR                      COLOR_TAN1
#define COLOR_SCHEME_BIT_LITERAL_COLOR                      COLOR_TAN1
#define COLOR_SCHEME_BOOL_LITERAL_COLOR                     COLOR_TAN1

void write_dot_header(FILE* handle) {
    assert(handle != NULL);

    fputs(""
        "digraph {\n"
        "\tranksep = 0.35\n"
        "\tnode [\n"
        "\t\tshape = \"box\",\n"
        "\t\tstyle = \"solid, filled\",\n"
        "\t\tfontcolor = \"dark\",\n"
        "\t\tfontsize = 12,\n"
        "\t\twidth = 0.5,\n"
        "\t\theight = 0.25\n"
        "\t];\n\n"
        "\tedge [\n"
        "\t\tarrowsize = 0.6,\n"
        "\t\tcolor = \"black\",\n"
        "\t\tstyle = \"light\"\n"
        "\t];\n\n",
        handle);
}

void write_ast_node_to_dot_file(FILE* file, const char* prev_node_name, const ASTNode* node) {
    assert(file != NULL && node != NULL);
    static u64 node_num = 0;
    char* node_name = str_format("node%d", ++node_num);

    switch (node->kind) {
    case AST_FUNCSIGN_NODE: {
        fprintf(file, "%s ["
            "label=\"SIGNATURE\", "
            "fillcolor=\""COLOR_SCHEME_FUNCSIGN_COLOR"\""
            "];\n", node_name);

        if (prev_node_name != NULL) {
            fprintf(file, "%s -> %s;\n", prev_node_name, node_name);
        }

        write_ast_node_to_dot_file(file, node_name, node->as.funcsign->id);

        if (node->as.funcsign->args.items_count > 0) {
            char* args = str_concat(node_name, "_args");
            fprintf(file, "%s ["
                "label=\"ARGS\", "
                "color=\""COLOR_SCHEME_FUNCSIGN_ARGS_COLOR"\", "
                "style=\"dashed\""
                "];\n", args);

            fprintf(file, "%s -> %s;\n", node_name, args);

            for (u64 i = 0; i < node->as.funcsign->args.items_count; ++i) {
                const ASTNode* arg = vector_get_ref(&node->as.funcsign->args, i);

                write_ast_node_to_dot_file(file, args, arg);
            }
            
            str_free(args);
        }

        if (node->as.funcsign->typeref != NULL) {
            char* rettype = str_concat(node_name, "_rettype");
            fprintf(file, "%s ["
                "label=\"TYPEREF\", "
                "color=\""COLOR_SCHEME_TYPEREF_COLOR"\", "
                "style=\"dashed\""
                "];\n", rettype);

            fprintf(file, "%s -> %s;\n", node_name, rettype);

            write_ast_node_to_dot_file(file, rettype, node->as.funcsign->typeref);
            
            str_free(rettype);
        }
    } break;
    case AST_ARGDEF_NODE: {
        fprintf(file, "%s ["
            "label=\"ARGDEF\", "
            "fillcolor=\""COLOR_SCHEME_ARGDEF_COLOR"\""
            "];\n", node_name);

        if (prev_node_name != NULL) {
            fprintf(file, "%s -> %s;\n", prev_node_name, node_name);
        }

        write_ast_node_to_dot_file(file, node_name, node->as.argdef->id);

        if (node->as.argdef->typeref != NULL) {
            char* typeref = str_concat(node_name, "_typeref");
            fprintf(file, "%s ["
                "label=\"TYPEREF\", "
                "color=\""COLOR_SCHEME_TYPEREF_COLOR"\", "
                "style=\"dashed\""
                "];\n", typeref);

            fprintf(file, "%s -> %s;\n", node_name, typeref);

            write_ast_node_to_dot_file(file, typeref, node->as.argdef->typeref);

            str_free(typeref);
        }
    } break;
    case AST_IDENTIFIER_NODE: {
        fprintf(file, "%s ["
            "label=\"ID\", "
            "fillcolor=\""COLOR_SCHEME_IDENTIFIER_COLOR"\""
            "];\n", node_name);

        if (prev_node_name != NULL) {
            fprintf(file, "%s -> %s;\n", prev_node_name, node_name);
        }

        char* value = str_concat(node_name, "_value");
        fprintf(file, "%s ["
            "label=\"%s\", "
            "color=\""COLOR_SCHEME_IDENTIFIER_VALUE_COLOR"\", "
            "style=\"dashed\""
            "];\n", value, node->as.identifier->value);

        fprintf(file, "%s -> %s;\n", node_name, value);

        str_free(value);
    } break;
    case AST_FUNCDEF_NODE: {
        fprintf(file, "%s ["
            "label=\"FUNCTION\", "
            "fillcolor=\""COLOR_SCHEME_FUNCDEF_STATEMENTS_COLOR"\""
            "];\n", node_name);

        if (prev_node_name != NULL) {
            fprintf(file, "%s -> %s;\n", prev_node_name, node_name);
        }

        write_ast_node_to_dot_file(file, node_name, node->as.funcdef->funcsign);

        if (node->as.funcdef->stmts.items_count > 0) {
            char* block = str_concat(node_name, "_block");
            fprintf(file, "%s ["
                "label=\"STATEMENTS\", "
                "color=\""COLOR_SCHEME_FUNCDEF_STATEMENTS_COLOR"\", "
                "style=\"dashed\""
                "];\n", block);

            fprintf(file, "%s -> %s;\n", node_name, block);

            for (u64 i = 0; i < node->as.funcdef->stmts.items_count; ++i) {
                const ASTNode* stmt = vector_get_ref(&node->as.funcdef->stmts, i);

                write_ast_node_to_dot_file(file, block, stmt);
            }

            str_free(block);
        }
    } break;
    case AST_BUILTIN_TYPEREF_NODE: {
        fprintf(file, "%s ["
            "label=\"BUILTIN\", "
            "fillcolor=\""COLOR_SCHEME_BUILTIN_TYPEREF_COLOR"\""
            "];\n", node_name);

        if (prev_node_name != NULL) {
            fprintf(file, "%s -> %s;\n", prev_node_name, node_name);
        }

        char* value = str_concat(node_name, "_value");
        fprintf(file, "%s ["
            "label=\"%s\", "
            "color=\""COLOR_SCHEME_BUILTIN_TYPEREF_VALUE_COLOR"\", "
            "style=\"dashed\""
            "];\n", value, node->as.builtin_typeref->value);

        fprintf(file, "%s -> %s;\n", node_name, value);

        str_free(value);
    } break;
    case AST_CUSTOM_TYPEREF_NODE: {
        fprintf(file, "%s ["
            "label=\"CUSTOM\", "
            "fillcolor=\""COLOR_SCHEME_CUSTOM_TYPEREF_COLOR"\""
            "];\n", node_name);

        if (prev_node_name != NULL) {
            fprintf(file, "%s -> %s;\n", prev_node_name, node_name);
        }

        write_ast_node_to_dot_file(file, node_name, node->as.custom_typeref->id);
    } break;
    case AST_ARRAY_TYPEREF_NODE: {
        fprintf(file, "%s ["
            "label=\"ARRAY\", "
            "fillcolor=\""COLOR_SCHEME_ARRAY_TYPEREF_COLOR"\""
            "];\n", node_name);

        if (prev_node_name != NULL) {
            fprintf(file, "%s -> %s;\n", prev_node_name, node_name);
        }

        char* dimension = str_concat(node_name, "_dimension");

        fprintf(file, "%s ["
            "label=\"DIM\", "
            "color=\""COLOR_SCHEME_BUILTIN_TYPEREF_VALUE_COLOR"\", "
            "style=\"dashed\""
            "];\n", dimension);

        fprintf(file, "%s -> %s;\n", node_name, dimension);

        char* value = str_concat(node_name, "_dimension_value");
        fprintf(file, "%s ["
            "label=\"%lld\", "
            "color=\""COLOR_SCHEME_ARRAY_TYPEREF_DIMENSION_VALUE_COLOR"\", "
            "style=\"dashed\""
            "];\n", value, node->as.array_typeref->dimension);

        fprintf(file, "%s -> %s;\n", dimension, value);

        write_ast_node_to_dot_file(file, node_name, node->as.array_typeref->typeref);

        str_free(dimension);
        str_free(value);
    } break;
    case AST_VARDECL_STMT_NODE: {
        fprintf(file, "%s ["
            "label=\"VAR DECL\", "
            "fillcolor=\""COLOR_SCHEME_VARDECL_STMT_COLOR"\""
            "];\n", node_name);

        if (prev_node_name != NULL) {
            fprintf(file, "%s -> %s;\n", prev_node_name, node_name);
        }

        write_ast_node_to_dot_file(file, node_name, node->as.vardecl_stmt->typeref);

        if (node->as.vardecl_stmt->ids.items_count > 0) {
            char* ids = str_concat(node_name, "_ids");
            fprintf(file, "%s ["
                "label=\"IDS\", "
                "color=\""COLOR_SCHEME_VARDECL_STMT_IDS_COLOR"\", "
                "style=\"dashed\""
                "];\n", ids);

            fprintf(file, "%s -> %s;\n", node_name, ids);

            for (u64 i = 0; i < node->as.vardecl_stmt->ids.items_count; ++i) {
                const ASTNode* id = vector_get_ref(&node->as.vardecl_stmt->ids, i);

                write_ast_node_to_dot_file(file, ids, id);
            }

            str_free(ids);
        }
    } break;
    case AST_CONDITION_STMT_NODE: {
        fprintf(file, "%s ["
            "label=\"CONDITION\", "
            "fillcolor=\""COLOR_SCHEME_CONDITION_STMT_COLOR"\""
            "];\n", node_name);

        if (prev_node_name != NULL) {
            fprintf(file, "%s -> %s;\n", prev_node_name, node_name);
        }

        char* expr = str_concat(node_name, "_expr");

        fprintf(file, "%s ["
            "label=\"EXPR\", "
            "fillcolor=\""COLOR_SCHEME_CONDITION_STMT_EXPR_COLOR"\""
            "];\n", expr);

        fprintf(file, "%s -> %s;\n", node_name, expr);

        write_ast_node_to_dot_file(file, expr, node->as.condition_stmt->expr);

        str_free(expr);

        if (node->as.condition_stmt->then_branch.items_count > 0) {
            char* then_branch = str_concat(node_name, "_then");
            fprintf(file, "%s ["
                "label=\"THEN\", "
                "color=\""COLOR_SCHEME_CONDITION_STMT_THEN_COLOR"\", "
                "style=\"dashed\""
                "];\n", then_branch);

            fprintf(file, "%s -> %s;\n", node_name, then_branch);

            for (u64 i = 0; i < node->as.condition_stmt->then_branch.items_count; ++i) {
                const ASTNode* stmt = vector_get_ref(&node->as.condition_stmt->then_branch, i);

                write_ast_node_to_dot_file(file, then_branch, stmt);
            }

            str_free(then_branch);
        }

        if (node->as.condition_stmt->else_branch.items_count > 0) {
            char* else_branch = str_concat(node_name, "_else");
            fprintf(file, "%s ["
                "label=\"ELSE\", "
                "color=\""COLOR_SCHEME_CONDITION_STMT_ELSE_COLOR"\", "
                "style=\"dashed\""
                "];\n", else_branch);

            fprintf(file, "%s -> %s;\n", node_name, else_branch);

            for (u64 i = 0; i < node->as.condition_stmt->else_branch.items_count; ++i) {
                const ASTNode* stmt = vector_get_ref(&node->as.condition_stmt->else_branch, i);

                write_ast_node_to_dot_file(file, else_branch, stmt);
            }

            str_free(else_branch);
        }
    } break;
    case AST_WHILE_STMT_NODE: {
        fprintf(file, "%s ["
            "label=\"WHILE\", "
            "fillcolor=\""COLOR_SCHEME_WHILE_STMT_COLOR"\""
            "];\n", node_name);

        if (prev_node_name != NULL) {
            fprintf(file, "%s -> %s;\n", prev_node_name, node_name);
        }

        char* expr = str_concat(node_name, "_expr");

        fprintf(file, "%s ["
            "label=\"EXPR\", "
            "fillcolor=\""COLOR_SCHEME_WHILE_STMT_EXPR_COLOR"\""
            "];\n", expr);

        fprintf(file, "%s -> %s;\n", node_name, expr);

        write_ast_node_to_dot_file(file, expr, node->as.while_stmt->expr);

        str_free(expr);

        if (node->as.while_stmt->stmts.items_count > 0) {
            char* stmts = str_concat(node_name, "_stmt");
            fprintf(file, "%s ["
                "label=\"STATEMENTS\", "
                "color=\""COLOR_SCHEME_WHILE_STMT_STATEMENTS_COLOR"\", "
                "style=\"dashed\""
                "];\n", stmts);

            fprintf(file, "%s -> %s;\n", node_name, stmts);

            for (u64 i = 0; i < node->as.while_stmt->stmts.items_count; ++i) {
                const ASTNode* stmt = vector_get_ref(&node->as.while_stmt->stmts, i);

                write_ast_node_to_dot_file(file, stmts, stmt);
            }

            str_free(stmts);
        }
    } break;
    case AST_DO_WHILE_STMT_NODE: {
        fprintf(file, "%s ["
            "label=\"DO WHILE\", "
            "fillcolor=\""COLOR_SCHEME_DO_WHILE_STMT_COLOR"\""
            "];\n", node_name);

        if (prev_node_name != NULL) {
            fprintf(file, "%s -> %s;\n", prev_node_name, node_name);
        }

        char* expr = str_concat(node_name, "_expr");

        fprintf(file, "%s ["
            "label=\"EXPR\", "
            "fillcolor=\""COLOR_SCHEME_DO_WHILE_STMT_EXPR_COLOR"\""
            "];\n", expr);

        fprintf(file, "%s -> %s;\n", node_name, expr);

        write_ast_node_to_dot_file(file, expr, node->as.while_stmt->expr);

        str_free(expr);

        if (node->as.while_stmt->stmts.items_count > 0) {
            char* stmts = str_concat(node_name, "_stmt");
            fprintf(file, "%s ["
                "label=\"STATEMENTS\", "
                "color=\""COLOR_SCHEME_DO_WHILE_STMT_STATEMENTS_COLOR"\", "
                "style=\"dashed\""
                "];\n", stmts);

            fprintf(file, "%s -> %s;\n", node_name, stmts);

            for (u64 i = 0; i < node->as.while_stmt->stmts.items_count; ++i) {
                const ASTNode* stmt = vector_get_ref(&node->as.while_stmt->stmts, i);

                write_ast_node_to_dot_file(file, stmts, stmt);
            }

            str_free(stmts);
        }
    } break;
    case AST_BREAK_STMT_NODE: {
        fprintf(file, "%s ["
            "label=\"BREAK\", "
            "fillcolor=\""COLOR_SCHEME_BREAK_STMT_COLOR"\""
            "];\n", node_name);

        if (prev_node_name != NULL) {
            fprintf(file, "%s -> %s;\n", prev_node_name, node_name);
        }
    } break;
    case AST_CONTINUE_STMT_NODE: {
        fprintf(file, "%s ["
            "label=\"CONTINUE\", "
            "fillcolor=\""COLOR_SCHEME_CONTINUE_STMT_COLOR"\""
            "];\n", node_name);

        if (prev_node_name != NULL) {
            fprintf(file, "%s -> %s;\n", prev_node_name, node_name);
        }
    } break;
    case AST_EXPRESSION_STMT_NODE: {
        fprintf(file, "%s ["
            "label=\"EXPESSION\", "
            "fillcolor=\""COLOR_SCHEME_EXPRESSION_STMT_COLOR"\""
            "];\n", node_name);

        if (prev_node_name != NULL) {
            fprintf(file, "%s -> %s;\n", prev_node_name, node_name);
        }

        write_ast_node_to_dot_file(file, node_name, node->as.expression_stmt->expr);
    } break;
    case AST_BINARY_EXPR_NODE: {
        fprintf(file, "%s ["
            "label=\"%s\", "
            "fillcolor=\""COLOR_SCHEME_BINARY_EXPR_COLOR"\""
            "];\n", node_name, node->as.binary_expr->op);

        if (prev_node_name != NULL) {
            fprintf(file, "%s -> %s;\n", prev_node_name, node_name);
        }

        write_ast_node_to_dot_file(file, node_name, node->as.binary_expr->lhs);
        write_ast_node_to_dot_file(file, node_name, node->as.binary_expr->rhs);
    } break;
    case AST_UNARY_EXPR_NODE: {
        fprintf(file, "%s ["
            "label=\"%s\", "
            "fillcolor=\""COLOR_SCHEME_UNARY_EXPR_COLOR"\""
            "];\n", node_name, node->as.unary_expr->op);

        if (prev_node_name != NULL) {
            fprintf(file, "%s -> %s;\n", prev_node_name, node_name);
        }

        write_ast_node_to_dot_file(file, node_name, node->as.unary_expr->rhs);
    } break;
    case AST_BRACES_EXPR_NODE: {
        write_ast_node_to_dot_file(file, prev_node_name, node->as.braces_expr->expr);
    } break;
    case AST_CALL_OR_INDEXER_EXPR_NODE: {
        fprintf(file, "%s ["
            "label=\"CALL\", "
            "fillcolor=\""COLOR_SCHEME_CALL_OR_INDEXER_EXPR_COLOR"\""
            "];\n", node_name);

        if (prev_node_name != NULL) {
            fprintf(file, "%s -> %s;\n", prev_node_name, node_name);
        }

        char* callee = str_concat(node_name, "_callee");

        fprintf(file, "%s ["
            "label=\"CALLEE\", "
            "color=\""COLOR_SCHEME_CALL_OR_INDEXER_EXPR_CALLEE_COLOR"\", "
            "style=\"dashed\""
            "];\n", callee);

        fprintf(file, "%s -> %s;\n", node_name, callee);

        write_ast_node_to_dot_file(file, callee, node->as.call_or_indexer_expr->callee);

        str_free(callee);

        if (node->as.call_or_indexer_expr->args.items_count > 0) {
            char* args = str_concat(node_name, "_args");
            fprintf(file, "%s ["
                "label=\"ARGS\", "
                "color=\""COLOR_SCHEME_CALL_OR_INDEXER_EXPR_ARGS_COLOR"\", "
                "style=\"dashed\""
                "];\n", args);

            fprintf(file, "%s -> %s;\n", node_name, args);

            for (u64 i = 0; i < node->as.call_or_indexer_expr->args.items_count; ++i) {
                const ASTNode* arg = vector_get_ref(&node->as.call_or_indexer_expr->args, i);

                write_ast_node_to_dot_file(file, args, arg);
            }

            str_free(args);
        }
    } break;
    case AST_PLACE_EXPR_NODE: {
        fprintf(file, "%s ["
            "label=\"PLACE\", "
            "fillcolor=\""COLOR_SCHEME_PLACE_EXPR_COLOR"\""
            "];\n", node_name);

        if (prev_node_name != NULL) {
            fprintf(file, "%s -> %s;\n", prev_node_name, node_name);
        }

        write_ast_node_to_dot_file(file, node_name, node->as.place_expr->id);
    } break;
    case AST_TERNARY_EXPR_NODE: {
        fprintf(file, "%s ["
            "label=\"TERNARY\", "
            "fillcolor=\""COLOR_SCHEME_TERNARY_EXPR_COLOR"\""
            "];\n", node_name);

        if (prev_node_name != NULL) {
            fprintf(file, "%s -> %s;\n", prev_node_name, node_name);
        }

        char* expr = str_concat(node_name, "_expr");

        fprintf(file, "%s ["
            "label=\"EXPR\", "
            "fillcolor=\""COLOR_SCHEME_TERNARY_EXPR_EXPR_COLOR"\""
            "];\n", expr);

        fprintf(file, "%s -> %s;\n", node_name, expr);

        write_ast_node_to_dot_file(file, expr, node->as.ternary_expr->expr);

        str_free(expr);

        char* then_branch = str_concat(node_name, "_then");
        fprintf(file, "%s ["
            "label=\"THEN\", "
            "color=\""COLOR_SCHEME_TERNARY_EXPR_THEN_COLOR"\", "
            "style=\"dashed\""
            "];\n", then_branch);

        fprintf(file, "%s -> %s;\n", node_name, then_branch);

        write_ast_node_to_dot_file(file, then_branch, node->as.ternary_expr->then_expr);

        str_free(then_branch);

        char* else_branch = str_concat(node_name, "_else");
        fprintf(file, "%s ["
            "label=\"ELSE\", "
            "color=\""COLOR_SCHEME_TERNARY_EXPR_ELSE_COLOR"\", "
            "style=\"dashed\""
            "];\n", else_branch);

        fprintf(file, "%s -> %s;\n", node_name, else_branch);

        write_ast_node_to_dot_file(file, else_branch, node->as.ternary_expr->else_expr);

        str_free(else_branch);
    } break;
    case AST_STRING_LITERAL_NODE: {
        fprintf(file, "%s ["
            "label=\"%s\", "
            "color=\""COLOR_SCHEME_STRING_LITERAL_COLOR"\", "
            "style=\"dashed\""
            "];\n", node_name, node->as.literal->value);

        if (prev_node_name != NULL) {
            fprintf(file, "%s -> %s;\n", prev_node_name, node_name);
        }
    } break;
    case AST_CHAR_LITERAL_NODE: {
        fprintf(file, "%s ["
            "label=\"%s\", "
            "color=\""COLOR_SCHEME_CHAR_LITERAL_COLOR"\", "
            "style=\"dashed\""
            "];\n", node_name, node->as.literal->value);

        if (prev_node_name != NULL) {
            fprintf(file, "%s -> %s;\n", prev_node_name, node_name);
        }
    } break;
    case AST_DEC_LITERAL_NODE: {
        fprintf(file, "%s ["
            "label=\"%s\", "
            "color=\""COLOR_SCHEME_DEC_LITERAL_COLOR"\", "
            "style=\"dashed\""
            "];\n", node_name, node->as.literal->value);

        if (prev_node_name != NULL) {
            fprintf(file, "%s -> %s;\n", prev_node_name, node_name);
        }
    } break;
    case AST_HEX_LITERAL_NODE: {
        fprintf(file, "%s ["
            "label=\"%s\", "
            "color=\""COLOR_SCHEME_HEX_LITERAL_COLOR"\", "
            "style=\"dashed\""
            "];\n", node_name, node->as.literal->value);

        if (prev_node_name != NULL) {
            fprintf(file, "%s -> %s;\n", prev_node_name, node_name);
        }
    } break;
    case AST_OCT_LITERAL_NODE: {
        fprintf(file, "%s ["
            "label=\"%s\", "
            "color=\""COLOR_SCHEME_OCT_LITERAL_COLOR"\", "
            "style=\"dashed\""
            "];\n", node_name, node->as.literal->value);

        if (prev_node_name != NULL) {
            fprintf(file, "%s -> %s;\n", prev_node_name, node_name);
        }
    } break;
    case AST_BITS_LITERAL_NODE: {
        fprintf(file, "%s ["
            "label=\"%s\", "
            "color=\""COLOR_SCHEME_BIT_LITERAL_COLOR"\", "
            "style=\"dashed\""
            "];\n", node_name, node->as.literal->value);

        if (prev_node_name != NULL) {
            fprintf(file, "%s -> %s;\n", prev_node_name, node_name);
        }
    } break;
    case AST_BOOL_LITERAL_NODE: {
        fprintf(file, "%s ["
            "label=\"%s\", "
            "color=\""COLOR_SCHEME_BOOL_LITERAL_COLOR"\", "
            "style=\"dashed\""
            "];\n", node_name, node->as.literal->value);

        if (prev_node_name != NULL) {
            fprintf(file, "%s -> %s;\n", prev_node_name, node_name);
        }
    } break;
    };

    str_free(node_name);
}