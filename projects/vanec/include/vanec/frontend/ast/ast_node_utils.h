#pragma once
#include <stdio.h>

#include "vanec/utils/vector.h"
#include "vanec/frontend/ast/ast_node.h"

void write_dot_header(FILE* handle);

void write_ast_node_to_dot_file(FILE* handle, const char* prev_node_name, const ASTNode* node);

bool write_ast_dot_file(const char* filepath, const Vector* functions);