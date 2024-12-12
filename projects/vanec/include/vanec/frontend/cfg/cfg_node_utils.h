#pragma once

#include "vanec/utils/vector.h"

#include "vanec/frontend/cfg/cfg_node.h"

bool write_cfg_dot_file(const char* filepath, CFGNode* cfg);