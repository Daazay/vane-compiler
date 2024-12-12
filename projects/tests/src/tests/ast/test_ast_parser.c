#include "utest/utest.h"

#include "vanec/frontend/ast/ast_parser.h"

struct ASTParserFixture {
    Stream* ss;
    Lexer* lexer;
    ASTParser* parser;
};

UTEST_F_SETUP(ASTParserFixture) {
    utest_fixture->ss = stream_create();
    utest_fixture->lexer = lexer_create(MIN_STREAM_CHUNK_CAPACITY, NULL);
    utest_fixture->parser = ast_parser_create(utest_fixture->lexer, NULL);
}

UTEST_F_TEARDOWN(ASTParserFixture) {
    ast_parser_free(utest_fixture->parser);
    lexer_free(utest_fixture->lexer);
    stream_free(utest_fixture->ss);
}

#pragma region DEFINES

#define ASSERT_IDENTIFIER(node, val)            \
ASSERT_NE(node, NULL);                          \
ASSERT_EQ(node->kind, AST_IDENTIFIER_NODE);     \
ASSERT_STREQ(node->as.identifier->value, val)

#define ASSERT_BUILTIN_TYPEREF(node, val)           \
ASSERT_NE(node, NULL);                              \
ASSERT_EQ(node->kind, AST_BUILTIN_TYPEREF_NODE);    \
ASSERT_STREQ(node->as.builtin_typeref->value, val)

#define ASSERT_CUSTOM_TYPEREF(node, val)            \
ASSERT_NE(node, NULL);                              \
ASSERT_EQ(node->kind, AST_CUSTOM_TYPEREF_NODE);     \
ASSERT_IDENTIFIER(node->as.custom_typeref->id, val) 

#define ASSERT_BUILTIN_ARRAY_TYPEREF(node, dim, val)        \
ASSERT_NE(node, NULL);                                      \
ASSERT_EQ(node->kind, AST_ARRAY_TYPEREF_NODE);              \
ASSERT_EQ(node->as.array_typeref->dimension, dim);          \
ASSERT_BUILTIN_TYPEREF(node->as.array_typeref->typeref, val) 

#define ASSERT_CUSTOM_ARRAY_TYPEREF(node, dim, val)         \
ASSERT_NE(node, NULL);                                      \
ASSERT_EQ(node->kind, AST_ARRAY_TYPEREF_NODE);              \
ASSERT_EQ(node->as.array_typeref->dimension, dim);          \
ASSERT_CUSTOM_TYPEREF(node->as.array_typeref->typeref, val) 

#define ASSERT_LITERAL(node, _kind, val)        \
ASSERT_NE(node, NULL);                          \
ASSERT_EQ(node->kind, _kind);                   \
ASSERT_STREQ(node->as.literal->value, val)

#define ASSERT_PLACE(node, val)                 \
ASSERT_NE(node, NULL);                          \
ASSERT_EQ(node->kind, AST_PLACE_EXPR_NODE);     \
ASSERT_IDENTIFIER(node->as.place_expr->id, val) 

#pragma endregion

UTEST_F(ASTParserFixture, identifier1) {
    const char* source = "identifier";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_identifier_node(utest_fixture->parser);

    ASSERT_IDENTIFIER(node, source);

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, identifier2) {
    const char* source = "";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_identifier_node(utest_fixture->parser);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, builtin_typeref1) {
    const char* source = "ulong";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_typeref_node(utest_fixture->parser);

    ASSERT_BUILTIN_TYPEREF(node, source);

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, builtin_typeref2) {
    const char* source = "int (";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_typeref_node(utest_fixture->parser);

    ASSERT_BUILTIN_TYPEREF(node, "int");

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, builtin_typeref3) {
    const char* source = "";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_typeref_node(utest_fixture->parser);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, custom_typeref1) {
    const char* source = "custom";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_typeref_node(utest_fixture->parser);

    ASSERT_CUSTOM_TYPEREF(node, source);

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, custom_typeref2) {
    const char* source = "custom (";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_typeref_node(utest_fixture->parser);

    ASSERT_CUSTOM_TYPEREF(node, "custom");

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, custom_typeref3) {
    const char* source = "";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_typeref_node(utest_fixture->parser);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, array_typeref1) {
    const char* source = "int()";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_typeref_node(utest_fixture->parser);

    ASSERT_BUILTIN_ARRAY_TYPEREF(node, 1, "int");

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, array_typeref2) {
    const char* source = "arr(,,,)";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_typeref_node(utest_fixture->parser);

    ASSERT_CUSTOM_ARRAY_TYPEREF(node, 4, "arr");

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, array_typeref3) {
    const char* source = "string(,)(,,)";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_typeref_node(utest_fixture->parser);

    ASSERT_NE(node, NULL);
    ASSERT_EQ(node->kind, AST_ARRAY_TYPEREF_NODE);

    u64 dimension = node->as.array_typeref->dimension;
    ASTNode* typeref = node->as.array_typeref->typeref;

    ASSERT_EQ(dimension, 3);
    ASSERT_BUILTIN_ARRAY_TYPEREF(typeref, 2, "string");

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, array_typeref4) {
    const char* source = "arr()(";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_typeref_node(utest_fixture->parser);

    ASSERT_CUSTOM_ARRAY_TYPEREF(node, 1, "arr");

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, array_typeref5) {
    const char* source = "";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_typeref_node(utest_fixture->parser);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, argdef1) {
    const char* source = "a";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_argdef_node(utest_fixture->parser);

    ASSERT_NE(node, NULL);
    ASSERT_EQ(node->kind, AST_ARGDEF_NODE);

    ASTNode* id = node->as.argdef->id;
    ASTNode* typeref = node->as.argdef->typeref;

    ASSERT_IDENTIFIER(id, "a");
    ASSERT_EQ(typeref, NULL);
}

UTEST_F(ASTParserFixture, argdef2) {
    const char* source = "a as int";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_argdef_node(utest_fixture->parser);

    ASSERT_NE(node, NULL);
    ASSERT_EQ(node->kind, AST_ARGDEF_NODE);

    ASTNode* id = node->as.argdef->id;
    ASTNode* typeref = node->as.argdef->typeref;

    ASSERT_IDENTIFIER(id, "a");
    ASSERT_BUILTIN_TYPEREF(typeref, "int");
}

UTEST_F(ASTParserFixture, argdef3) {
    const char* source = "a as";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_argdef_node(utest_fixture->parser);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, funcsign1) {
    const char* source = "main()";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_funcsign_node(utest_fixture->parser);

    ASSERT_NE(node, NULL);
    ASSERT_EQ(node->kind, AST_FUNCSIGN_NODE);

    ASTNode* id = node->as.funcsign->id;
    Vector* args = &node->as.funcsign->args;
    ASTNode* typeref = node->as.funcsign->typeref;

    ASSERT_IDENTIFIER(id, "main");
    ASSERT_EQ(args->items_count, 0);
    ASSERT_EQ(typeref, NULL);

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, funcsign2) {
    const char* source = "main(a, b)";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_funcsign_node(utest_fixture->parser);
    ASSERT_EQ(node->kind, AST_FUNCSIGN_NODE);

    ASSERT_NE(node, NULL);

    ASTNode* id = node->as.funcsign->id;
    Vector* args = &node->as.funcsign->args;
    ASTNode* typeref = node->as.funcsign->typeref;

    ASSERT_IDENTIFIER(id, "main");
    ASSERT_EQ(args->items_count, 2);
    ASSERT_EQ(typeref, NULL);

    ASTNode* arg1 = vector_get_ref(args, 0);
    ASTNode* arg2 = vector_get_ref(args, 1);

    ASSERT_NE(arg1, NULL);
    ASSERT_IDENTIFIER(arg1->as.argdef->id, "a");
    ASSERT_EQ(arg1->as.argdef->typeref, NULL);

    ASSERT_NE(arg2, NULL);
    ASSERT_IDENTIFIER(arg2->as.argdef->id, "b");
    ASSERT_EQ(arg2->as.argdef->typeref, NULL);

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, funcsign3) {
    const char* source = "main(a as int, b as arr())";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_funcsign_node(utest_fixture->parser);

    ASSERT_NE(node, NULL);
    ASSERT_EQ(node->kind, AST_FUNCSIGN_NODE);

    ASTNode* id = node->as.funcsign->id;
    Vector* args = &node->as.funcsign->args;
    ASTNode* typeref = node->as.funcsign->typeref;

    ASSERT_IDENTIFIER(id, "main");
    ASSERT_EQ(args->items_count, 2);
    ASSERT_EQ(typeref, NULL);

    ASTNode* arg1 = vector_get_ref(args, 0);
    ASTNode* arg2 = vector_get_ref(args, 1);

    ASSERT_NE(arg1, NULL);
    ASSERT_IDENTIFIER(arg1->as.argdef->id, "a");
    ASSERT_BUILTIN_TYPEREF(arg1->as.argdef->typeref, "int");

    ASSERT_NE(arg2, NULL);
    ASSERT_IDENTIFIER(arg2->as.argdef->id, "b");
    ASSERT_CUSTOM_ARRAY_TYPEREF(arg2->as.argdef->typeref, 1, "arr");

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, funcsign4) {
    const char* source = "main() as int";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_funcsign_node(utest_fixture->parser);

    ASSERT_NE(node, NULL);
    ASSERT_EQ(node->kind, AST_FUNCSIGN_NODE);

    ASTNode* id = node->as.funcsign->id;
    Vector* args = &node->as.funcsign->args;
    ASTNode* typeref = node->as.funcsign->typeref;

    ASSERT_IDENTIFIER(id, "main");
    ASSERT_EQ(args->items_count, 0);
    ASSERT_BUILTIN_TYPEREF(typeref, "int");

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, funcsign5) {
    const char* source = "main() as ";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_funcsign_node(utest_fixture->parser);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, funcsign6) {
    const char* source = "main(a, )";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_funcsign_node(utest_fixture->parser);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, funcsign7) {
    const char* source = "";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_funcsign_node(utest_fixture->parser);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, vardecl1) {
    const char* source = "dim a as int";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_vardecl_stmt_node(utest_fixture->parser);

    ASSERT_NE(node, NULL);
    ASSERT_EQ(node->kind, AST_VARDECL_STMT_NODE);

    Vector* ids = &node->as.vardecl_stmt->ids;
    ASTNode* typeref = node->as.vardecl_stmt->typeref;

    ASSERT_EQ(ids->items_count, 1);

    ASTNode* id = vector_get_ref(ids, 0);

    ASSERT_IDENTIFIER(id, "a");

    ASSERT_BUILTIN_TYPEREF(typeref, "int");

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, vardecl2) {
    const char* source = "dim a, b as int";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_vardecl_stmt_node(utest_fixture->parser);

    ASSERT_NE(node, NULL);
    ASSERT_EQ(node->kind, AST_VARDECL_STMT_NODE);

    Vector* ids = &node->as.vardecl_stmt->ids;
    ASTNode* typeref = node->as.vardecl_stmt->typeref;

    ASSERT_EQ(ids->items_count, 2);

    ASTNode* id1 = vector_get_ref(ids, 0);
    ASSERT_IDENTIFIER(id1, "a");

    ASTNode* id2 = vector_get_ref(ids, 1);
    ASSERT_IDENTIFIER(id2, "b");

    ASSERT_BUILTIN_TYPEREF(typeref, "int");

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, vardecl3) {
    const char* source = "dim a as";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_vardecl_stmt_node(utest_fixture->parser);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, vardecl4) {
    const char* source = "dim a, as int";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_vardecl_stmt_node(utest_fixture->parser);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, vardecl5) {
    const char* source = "dim as int";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_vardecl_stmt_node(utest_fixture->parser);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, string_literal1) {
    const char* source = "\"fox\"";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_NE(node, NULL);
    ASSERT_LITERAL(node, AST_STRING_LITERAL_NODE, "fox");

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, string_literal2) {
    const char* source = "\"fox";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, char_literal1) {
    const char* source = "\'a\'";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_NE(node, NULL);
    ASSERT_LITERAL(node, AST_CHAR_LITERAL_NODE, "a");

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, char_literal2) {
    const char* source = "\'a";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, char_literal3) {
    const char* source = "\'\'";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, char_literal4) {
    const char* source = "\'abcdefg\'";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, char_literal5) {
    const char* source = "";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, dec_literal1) {
    const char* source = "12345";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_NE(node, NULL);
    ASSERT_LITERAL(node, AST_DEC_LITERAL_NODE, "12345");

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, dec_literal2) {
    const char* source = "";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, hex_literal1) {
    const char* source = "0x12FF";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_NE(node, NULL);
    ASSERT_LITERAL(node, AST_HEX_LITERAL_NODE, "0x12FF");

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, hex_literal2) {
    const char* source = "0x12FFU";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, hex_literal3) {
    const char* source = "0x";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, hex_literal4) {
    const char* source = "";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, oct_literal1) {
    const char* source = "0123";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_NE(node, NULL);
    ASSERT_LITERAL(node, AST_OCT_LITERAL_NODE, "0123");

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, oct_literal2) {
    const char* source = "01238";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, oct_literal3) {
    const char* source = "";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, bits_literal1) {
    const char* source = "0b1001";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_NE(node, NULL);
    ASSERT_LITERAL(node, AST_BITS_LITERAL_NODE, "0b1001");

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, bits_literal2) {
    const char* source = "0b10013";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, bits_literal3) {
    const char* source = "0b";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, bits_literal4) {
    const char* source = "";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, bool_literal1) {
    const char* source = "true";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_NE(node, NULL);
    ASSERT_LITERAL(node, AST_BOOL_LITERAL_NODE, "true");

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, bool_literal2) {
    const char* source = "";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, place1) {
    const char* source = "a";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_PLACE(node, "a");

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, place2) {
    const char* source = "";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, unary1) {
    const char* source = "-10";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_NE(node, NULL);
    ASSERT_EQ(node->kind, AST_UNARY_EXPR_NODE);

    const char* op = node->as.unary_expr->op;
    const ASTNode* rhs = node->as.unary_expr->rhs;

    ASSERT_STREQ(op, "-");
    ASSERT_LITERAL(rhs, AST_DEC_LITERAL_NODE, "10");

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, unary2) {
    const char* source = "-a";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_NE(node, NULL);
    ASSERT_EQ(node->kind, AST_UNARY_EXPR_NODE);

    const char* op = node->as.unary_expr->op;
    const ASTNode* rhs = node->as.unary_expr->rhs;

    ASSERT_STREQ(op, "-");
    ASSERT_PLACE(rhs, "a");

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, unary3) {
    const char* source = "-";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, unary4) {
    const char* source = "";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, binary1) {
    const char* source = "5 - 6";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_NE(node, NULL);
    ASSERT_EQ(node->kind, AST_BINARY_EXPR_NODE);

    const char* op = node->as.binary_expr->op;
    const ASTNode* lhs = node->as.binary_expr->lhs;
    const ASTNode* rhs = node->as.binary_expr->rhs;

    ASSERT_STREQ(op, "-");
    ASSERT_LITERAL(lhs, AST_DEC_LITERAL_NODE, "5");
    ASSERT_LITERAL(rhs, AST_DEC_LITERAL_NODE, "6");

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, binary2) {
    const char* source = "5 -";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, binary3) {
    const char* source = "";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, braces1) {
    const char* source = "(10)";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_NE(node, NULL);
    ASSERT_EQ(node->kind, AST_BRACES_EXPR_NODE);

    const ASTNode* expr = node->as.braces_expr->expr;

    ASSERT_LITERAL(expr, AST_DEC_LITERAL_NODE, "10");

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, braces2) {
    const char* source = "(10";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, braces3) {
    const char* source = "()";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, braces4) {
    const char* source = "";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, call1) {
    const char* source = "call()";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_NE(node, NULL);
    ASSERT_EQ(node->kind, AST_CALL_OR_INDEXER_EXPR_NODE);

    const ASTNode* callee = node->as.call_or_indexer_expr->callee;
    const Vector* args = &node->as.call_or_indexer_expr->args;

    ASSERT_PLACE(callee, "call");
    ASSERT_EQ(args->items_count, 0);

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, call2) {
    const char* source = "call(a)";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_NE(node, NULL);
    ASSERT_EQ(node->kind, AST_CALL_OR_INDEXER_EXPR_NODE);

    const ASTNode* callee = node->as.call_or_indexer_expr->callee;
    const Vector* args = &node->as.call_or_indexer_expr->args;

    ASSERT_PLACE(callee, "call");
    ASSERT_EQ(args->items_count, 1);

    const ASTNode* arg = vector_get_ref(args, 0);

    ASSERT_PLACE(arg, "a");

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, call3) {
    const char* source = "call(a, b)";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_NE(node, NULL);
    ASSERT_EQ(node->kind, AST_CALL_OR_INDEXER_EXPR_NODE);

    const ASTNode* callee = node->as.call_or_indexer_expr->callee;
    const Vector* args = &node->as.call_or_indexer_expr->args;

    ASSERT_PLACE(callee, "call");
    ASSERT_EQ(args->items_count, 2);

    const ASTNode* arg1 = vector_get_ref(args, 0);
    ASSERT_PLACE(arg1, "a");

    const ASTNode* arg2 = vector_get_ref(args, 1);
    ASSERT_PLACE(arg2, "b");

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, call4) {
    const char* source = "call(a, b, c())";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_NE(node, NULL);
    ASSERT_EQ(node->kind, AST_CALL_OR_INDEXER_EXPR_NODE);

    const ASTNode* callee = node->as.call_or_indexer_expr->callee;
    const Vector* args = &node->as.call_or_indexer_expr->args;

    ASSERT_PLACE(callee, "call");
    ASSERT_EQ(args->items_count, 3);

    const ASTNode* arg1 = vector_get_ref(args, 0);
    ASSERT_PLACE(arg1, "a");

    const ASTNode* arg2 = vector_get_ref(args, 1);
    ASSERT_PLACE(arg2, "b");

    const ASTNode* arg3 = vector_get_ref(args, 2);

    ASSERT_NE(node, NULL);
    ASSERT_EQ(node->kind, AST_CALL_OR_INDEXER_EXPR_NODE);

    const ASTNode* arg3_callee = arg3->as.call_or_indexer_expr->callee;
    const Vector* arg3_args = &arg3->as.call_or_indexer_expr->args;

    ASSERT_PLACE(arg3_callee, "c");
    ASSERT_EQ(arg3_args->items_count, 0);

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, call5) {
    const char* source = "call(a(b, c))";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_NE(node, NULL);
    ASSERT_EQ(node->kind, AST_CALL_OR_INDEXER_EXPR_NODE);

    const ASTNode* callee = node->as.call_or_indexer_expr->callee;
    const Vector* args = &node->as.call_or_indexer_expr->args;

    ASSERT_PLACE(callee, "call");
    ASSERT_EQ(args->items_count, 1);

    const ASTNode* a = vector_get_ref(args, 0);

    ASSERT_NE(a, NULL);
    ASSERT_EQ(a->kind, AST_CALL_OR_INDEXER_EXPR_NODE);

    const ASTNode* callee1 = a->as.call_or_indexer_expr->callee;
    const Vector* args1 = &a->as.call_or_indexer_expr->args;

    ASSERT_PLACE(callee1, "a");
    ASSERT_EQ(args1->items_count, 2);

    const ASTNode* a_arg1 = vector_get_ref(args1, 0);
    ASSERT_PLACE(a_arg1, "b");

    const ASTNode* a_arg2 = vector_get_ref(args1, 1);
    ASSERT_PLACE(a_arg2, "c");

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, call6) {
    const char* source = "call(a, )";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, call7) {
    const char* source = "call(a";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, call8) {
    const char* source = "";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, ternary1) {
    const char* source = "a ? b : c";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_NE(node, NULL);
    ASSERT_EQ(node->kind, AST_TERNARY_EXPR_NODE);

    const ASTNode* expr = node->as.ternary_expr->expr;
    const ASTNode* then_expr = node->as.ternary_expr->then_expr;
    const ASTNode* else_expr = node->as.ternary_expr->else_expr;

    ASSERT_PLACE(expr, "a");
    ASSERT_PLACE(then_expr, "b");
    ASSERT_PLACE(else_expr, "c");

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, ternary2) {
    const char* source = "a ? b - c : -c";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_NE(node, NULL);
    ASSERT_EQ(node->kind, AST_TERNARY_EXPR_NODE);

    const ASTNode* expr = node->as.ternary_expr->expr;
    const ASTNode* then_expr = node->as.ternary_expr->then_expr;
    const ASTNode* else_expr = node->as.ternary_expr->else_expr;

    ASSERT_PLACE(node->as.ternary_expr->expr, "a");

    ASSERT_NE(then_expr, NULL);
    ASSERT_EQ(then_expr->kind, AST_BINARY_EXPR_NODE);

    const char* op1 = then_expr->as.binary_expr->op;
    const ASTNode* lhs1 = then_expr->as.binary_expr->lhs;
    const ASTNode* rhs1 = then_expr->as.binary_expr->rhs;

    ASSERT_STREQ(op1, "-");
    ASSERT_PLACE(lhs1, "b");
    ASSERT_PLACE(rhs1, "c");

    ASSERT_NE(else_expr, NULL);
    ASSERT_EQ(else_expr->kind, AST_UNARY_EXPR_NODE);

    const char* op2 = else_expr->as.unary_expr->op;
    const ASTNode* rhs2 = else_expr->as.unary_expr->rhs;

    ASSERT_STREQ(op2, "-");
    ASSERT_PLACE(rhs2, "c");

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, ternary3) {
    const char* source = "res = a > b ? a - b : b * a";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_NE(node, NULL);
    ASSERT_EQ(node->kind, AST_BINARY_EXPR_NODE);

    const char* op = node->as.binary_expr->op;
    const ASTNode* lhs = node->as.binary_expr->lhs;
    const ASTNode* rhs = node->as.binary_expr->rhs;

    ASSERT_STREQ(op, "=");
    ASSERT_PLACE(lhs, "res");

    ASSERT_NE(rhs, NULL);
    ASSERT_EQ(rhs->kind, AST_TERNARY_EXPR_NODE);

    const ASTNode* expr = rhs->as.ternary_expr->expr;
    const ASTNode* then_expr = rhs->as.ternary_expr->then_expr;
    const ASTNode* else_expr = rhs->as.ternary_expr->else_expr;

    ASSERT_NE(expr, NULL);
    ASSERT_EQ(expr->kind, AST_BINARY_EXPR_NODE);

    const char* op1 = expr->as.binary_expr->op;
    const ASTNode* lhs1 = expr->as.binary_expr->lhs;
    const ASTNode* rhs1 = expr->as.binary_expr->rhs;

    ASSERT_STREQ(op1, ">");
    ASSERT_PLACE(lhs1, "a");
    ASSERT_PLACE(rhs1, "b");

    ASSERT_NE(then_expr, NULL);
    ASSERT_EQ(then_expr->kind, AST_BINARY_EXPR_NODE);
    
    const char* op2 = then_expr->as.binary_expr->op;
    const ASTNode* lhs2 = then_expr->as.binary_expr->lhs;
    const ASTNode* rhs2 = then_expr->as.binary_expr->rhs;

    ASSERT_STREQ(op2, "-");
    ASSERT_PLACE(lhs2, "a");
    ASSERT_PLACE(rhs2, "b");

    ASSERT_NE(else_expr, NULL);
    ASSERT_EQ(else_expr->kind, AST_BINARY_EXPR_NODE);

    const char* op3 = else_expr->as.binary_expr->op;
    const ASTNode* lhs3 = else_expr->as.binary_expr->lhs;
    const ASTNode* rhs3 = else_expr->as.binary_expr->rhs;

    ASSERT_STREQ(op3, "*");
    ASSERT_PLACE(lhs3, "b");
    ASSERT_PLACE(rhs3, "a");

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, ternary4) {
    const char* source = "a ? b :";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, ternary5) {
    const char* source = "a ?  : c";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, ternary6) {
    const char* source = "a ? ";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, ternary7) {
    const char* source = "";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, condition1) {
    const char* source = ""
        "if true then\n"
        "end if\n"
        ;
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_condition_stmt_node(utest_fixture->parser);

    ASSERT_NE(node, NULL);
    ASSERT_EQ(node->kind, AST_CONDITION_STMT_NODE);

    const ASTNode* expr = node->as.condition_stmt->expr;
    const Vector* then_branch = &node->as.condition_stmt->then_branch;
    const Vector* else_branch = &node->as.condition_stmt->else_branch;

    ASSERT_LITERAL(expr, AST_BOOL_LITERAL_NODE, "true");
    ASSERT_EQ(then_branch->items_count, 0);
    ASSERT_EQ(else_branch->items_count, 0);

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, condition2) {
    const char* source = ""
        "if true then\n"
        "   call();\n"
        "end if\n"
        ;
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_condition_stmt_node(utest_fixture->parser);

    ASSERT_NE(node, NULL);
    ASSERT_EQ(node->kind, AST_CONDITION_STMT_NODE);

    const ASTNode* expr = node->as.condition_stmt->expr;
    const Vector* then_branch = &node->as.condition_stmt->then_branch;
    const Vector* else_branch = &node->as.condition_stmt->else_branch;

    ASSERT_LITERAL(expr, AST_BOOL_LITERAL_NODE, "true");
    ASSERT_EQ(then_branch->items_count, 1);

    const ASTNode* stmt = vector_get_ref(then_branch, 0);

    ASSERT_EQ(stmt->kind, AST_EXPRESSION_STMT_NODE);

    const ASTNode* call = stmt->as.expression_stmt->expr;

    ASSERT_NE(call, NULL);
    ASSERT_EQ(call->kind, AST_CALL_OR_INDEXER_EXPR_NODE);

    const ASTNode* callee = call->as.call_or_indexer_expr->callee;
    const Vector* args = &call->as.call_or_indexer_expr->args;

    ASSERT_PLACE(callee, "call");
    ASSERT_EQ(args->items_count, 0);

    ASSERT_EQ(else_branch->items_count, 0);

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, condition3) {
    const char* source = ""
        "if true then\n"
        "   call1();\n"
        "else\n"
        "   call2();\n"
        "end if\n"
        ;
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_condition_stmt_node(utest_fixture->parser);

    ASSERT_NE(node, NULL);
    ASSERT_EQ(node->kind, AST_CONDITION_STMT_NODE);

    const ASTNode* expr = node->as.condition_stmt->expr;
    const Vector* then_branch = &node->as.condition_stmt->then_branch;
    const Vector* else_branch = &node->as.condition_stmt->else_branch;

    ASSERT_LITERAL(expr, AST_BOOL_LITERAL_NODE, "true");
    ASSERT_EQ(then_branch->items_count, 1);

    const ASTNode* stmt = vector_get_ref(then_branch, 0);

    ASSERT_EQ(stmt->kind, AST_EXPRESSION_STMT_NODE);

    const ASTNode* call1 = stmt->as.expression_stmt->expr;

    ASSERT_NE(call1, NULL);
    ASSERT_EQ(call1->kind, AST_CALL_OR_INDEXER_EXPR_NODE);

    const ASTNode* callee = call1->as.call_or_indexer_expr->callee;
    const Vector* args = &call1->as.call_or_indexer_expr->args;

    ASSERT_PLACE(callee, "call1");
    ASSERT_EQ(args->items_count, 0);

    ASSERT_EQ(else_branch->items_count, 1);

    const ASTNode* stmt2 = vector_get_ref(else_branch, 0);

    ASSERT_EQ(stmt2->kind, AST_EXPRESSION_STMT_NODE);

    const ASTNode* call2 = stmt2->as.expression_stmt->expr;

    ASSERT_NE(call2, NULL);
    ASSERT_EQ(call2->kind, AST_CALL_OR_INDEXER_EXPR_NODE);

    const ASTNode* callee2 = call2->as.call_or_indexer_expr->callee;
    const Vector* args2 = &call2->as.call_or_indexer_expr->args;

    ASSERT_PLACE(callee2, "call2");
    ASSERT_EQ(args2->items_count, 0);

    ast_node_free(node);
}

UTEST_F(ASTParserFixture, condition4) {
    const char* source = "if true then"
        "end";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, condition5) {
    const char* source = "if true "
        "end if";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

UTEST_F(ASTParserFixture, condition6) {
    const char* source = "if then"
        "end if";
    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
    lexer_set_source_stream(utest_fixture->lexer, utest_fixture->ss);

    ASTNode* node = ast_parser_parse_ast_expression_node(utest_fixture->parser, PREC_NONE);

    ASSERT_EQ(node, NULL);
}

//UTEST_F(ASTParserFixture, while_stmt_valid) {
//    const char* source = ""
//        "while true\n"
//        "   print(a);\n"
//        "wend"
//        ;
//    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
//
//    ASTNode* node = ast_parser_parse_ast_while_stmt_node(utest_fixture->parser);
//
//    ASSERT_NE(node, NULL);
//    ASSERT_EQ(node->kind, AST_WHILE_STMT_NODE);
//
//    ASSERT_LITERAL(node->as.while_stmt->expr, AST_BOOL_LITERAL_NODE, "true");
//
//    ASSERT_EQ(node->as.while_stmt->stmts.items_count, 1);
//
//    const ASTNode* stmt = vector_get_ref(&node->as.while_stmt->stmts, 0);
//
//    ASSERT_NE(stmt, NULL);
//    ASSERT_EQ(stmt->kind, AST_EXPRESSION_STMT_NODE);
//
//    const ASTNode* expr = stmt->as.expression_stmt->expr;
//
//    ASSERT_PLACE(expr->as.call_or_indexer_expr->callee, "print");
//
//    ASSERT_EQ(expr->as.call_or_indexer_expr->args.items_count, 1);
//
//    const ASTNode* arg1 = vector_get_ref(&expr->as.call_or_indexer_expr->args, 0);
//
//    ASSERT_PLACE(arg1, "a");
//
//    ast_node_free(node);
//}
//
//UTEST_F(ASTParserFixture, while_stmt_invalid1) {
//    const char* source = "";
//    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
//
//    ASTNode* node = ast_parser_parse_ast_while_stmt_node(utest_fixture->parser);
//
//    ASSERT_EQ(node, NULL);
//}
//
//UTEST_F(ASTParserFixture, while_stmt_invalid2) {
//    const char* source = ""
//        "while true\n"
//        "   print(a);\n"
//        ""
//        ;
//    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
//
//    ASTNode* node = ast_parser_parse_ast_while_stmt_node(utest_fixture->parser);
//
//    ASSERT_EQ(node, NULL);
//}
//
//UTEST_F(ASTParserFixture, do_while_stmt_valid) {
//    const char* source = ""
//        "do\n"
//        "   print(a);\n"
//        "loop until true"
//        ;
//    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
//
//    ASTNode* node = ast_parser_parse_ast_do_while_stmt_node(utest_fixture->parser);
//
//    ASSERT_NE(node, NULL);
//    ASSERT_EQ(node->kind, AST_DO_WHILE_STMT_NODE);
//
//    ASSERT_LITERAL(node->as.while_stmt->expr, AST_BOOL_LITERAL_NODE, "true");
//
//    ASSERT_EQ(node->as.while_stmt->stmts.items_count, 1);
//
//    const ASTNode* stmt = vector_get_ref(&node->as.while_stmt->stmts, 0);
//
//    ASSERT_NE(stmt, NULL);
//    ASSERT_EQ(stmt->kind, AST_EXPRESSION_STMT_NODE);
//
//    const ASTNode* expr = stmt->as.expression_stmt->expr;
//
//    ASSERT_PLACE(expr->as.call_or_indexer_expr->callee, "print");
//
//    ASSERT_EQ(expr->as.call_or_indexer_expr->args.items_count, 1);
//
//    const ASTNode* arg1 = vector_get_ref(&expr->as.call_or_indexer_expr->args, 0);
//
//    ASSERT_PLACE(arg1, "a");
//
//    ast_node_free(node);
//}
//
//UTEST_F(ASTParserFixture, do_while_stmt_invalid1) {
//    const char* source = "";
//    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
//
//    ASTNode* node = ast_parser_parse_ast_while_stmt_node(utest_fixture->parser);
//
//    ASSERT_EQ(node, NULL);
//}
//
//UTEST_F(ASTParserFixture, do_while_stmt_invalid2) {
//    const char* source = ""
//        "do"
//        ""
//        ;
//    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
//
//    ASTNode* node = ast_parser_parse_ast_while_stmt_node(utest_fixture->parser);
//
//    ASSERT_EQ(node, NULL);
//}
//
//UTEST_F(ASTParserFixture, do_while_stmt_invalid3) {
//    const char* source = ""
//        "do\n"
//        "loop"
//        ;
//    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
//
//    ASTNode* node = ast_parser_parse_ast_while_stmt_node(utest_fixture->parser);
//
//    ASSERT_EQ(node, NULL);
//}
//
//UTEST_F(ASTParserFixture, do_while_stmt_invalid4) {
//    const char* source = ""
//        "do\n"
//        "loop true"
//        ;
//    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
//
//    ASTNode* node = ast_parser_parse_ast_while_stmt_node(utest_fixture->parser);
//
//    ASSERT_EQ(node, NULL);
//}
//
//UTEST_F(ASTParserFixture, break_stmt_valid) {
//    const char* source = "break";
//    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
//
//    ASTNode* node = ast_parser_parse_ast_break_stmt_node(utest_fixture->parser);
//
//    ASSERT_NE(node, NULL);
//    ASSERT_EQ(node->kind, AST_BREAK_STMT_NODE);
//
//    ast_node_free(node);
//}
//
//UTEST_F(ASTParserFixture, continue_stmt_valid) {
//    const char* source = "continue";
//    stream_set_source(utest_fixture->ss, STREAM_STRING_SOURCE, source);
//
//    ASTNode* node = ast_parser_parse_ast_continue_stmt_node(utest_fixture->parser);
//
//    ASSERT_NE(node, NULL);
//    ASSERT_EQ(node->kind, AST_CONTINUE_STMT_NODE);
//
//    ast_node_free(node);
//}