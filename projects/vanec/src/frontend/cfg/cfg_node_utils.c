#include "vanec/frontend/cfg/cfg_node_utils.h"

#include <assert.h>
#include <stdio.h>

#include "vanec/utils/string_utils.h"
#include "vanec/utils/string_builder.h"

static void write_dot_header(FILE* handle) {
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

static void write_cfg_node_decl(FILE* handle, const CFGNode* node) {
    assert(node != NULL);

    switch (node->kind) {
    case CFG_FUNC_ENTRY_NODE: {
        fprintf(handle, "\tnode%ld [label=\"%ld:func_entry\", fillcolor=\"palegreen\"];\n", node->id, node->id);
    } break;
    case CFG_FUNC_EXIT_NODE: {
        fprintf(handle, "\tnode%ld [label=\"%ld:func_exit\", fillcolor=\"tomato\"];\n", node->id, node->id);
    } break;
    case CFG_BASIC_BLOCK_NODE: {
        fprintf(handle, "\tnode%ld [label=\"%ld:bb\"];\n", node->id, node->id);
    } break;
    case CFG_CONDITION_NODE: {
        fprintf(handle, "\tnode%ld [label=\"%ld:condition\", fillcolor=\"lemonchiffon\"];\n", node->id, node->id);
    } break;
    case CFG_LOOP_ENTRY_NODE: {
        fprintf(handle, "\tnode%ld [label=\"%ld:loop_entry\", fillcolor=\"lightskyblue\"];\n", node->id, node->id);
    } break;
    case CFG_LOOP_EXIT_NODE: {
        fprintf(handle, "\tnode%ld [label=\"%ld:loop_exit\", fillcolor=\"lightskyblue\"];\n", node->id, node->id);
    } break;
    case CFG_BACKEDGE_NODE: {
        fprintf(handle, "\tnode%ld [label=\"%ld:backedge\", style=\"dashed\"];\n", node->id, node->id);
    } break;
    case CFG_BREAK_NODE: {
        fprintf(handle, "\tnode%ld [label=\"%ld:break\", fillcolor=\"tomato\", style=\"dashed\"];\n", node->id, node->id);
    } break;
    default: {
        assert(false && "unreachable");
    }
    };
}

static void write_cfg_node_to_dot_file(
    FILE* handle,
    const u64 visited_count,
    const u32 prev_node_id,
    const char* edge_label,
    const char* edge_color,
    const char* edge_style,
    CFGNode* node
) {
    assert(handle != NULL && node != NULL);

    switch (node->kind) {
    case CFG_FUNC_ENTRY_NODE: {
        if (visited_count != node->visited) {
            break;
        }
        ++node->visited;

        write_cfg_node_decl(handle, node);

        write_cfg_node_to_dot_file(handle, visited_count, node->id, "", "", "", node->as.func_entry->block.next);
    } break;
    case CFG_FUNC_EXIT_NODE: {
        fprintf(handle, "\tnode%ld -> node%ld [label=\"%s\", color=\"%s\", style=\"%s\"];\n", prev_node_id, node->id, edge_label, edge_color, edge_style);
        if (visited_count != node->visited) {
            break;
        }
        ++node->visited;

        write_cfg_node_decl(handle, node);
    } break;
    case CFG_BASIC_BLOCK_NODE: {
        fprintf(handle, "\tnode%ld -> node%ld [label=\"%s\", color=\"%s\", style=\"%s\"];\n", prev_node_id, node->id, edge_label, edge_color, edge_style);
        if (visited_count != node->visited) {
            break;
        }
        ++node->visited;

        write_cfg_node_decl(handle, node);

        write_cfg_node_to_dot_file(handle, visited_count, node->id, "", "", "", node->as.basic_block->next);
    } break;
    case CFG_CONDITION_NODE: {
        fprintf(handle, "\tnode%ld -> node%ld [label=\"%s\", color=\"%s\", style=\"%s\"];\n", prev_node_id, node->id, edge_label, edge_color, edge_style);
        if (visited_count != node->visited) {
            break;
        }
        ++node->visited;

        write_cfg_node_decl(handle, node);

        write_cfg_node_to_dot_file(handle, visited_count, node->id, "true", "palegreen", "", node->as.condition->then_branch.next);
        write_cfg_node_to_dot_file(handle, visited_count, node->id, "false", "tomato", "", node->as.condition->else_branch.next);
    } break;
    case CFG_LOOP_ENTRY_NODE: {
        fprintf(handle, "\tnode%ld -> node%ld [label=\"%s\", color=\"%s\", style=\"%s\"];\n", prev_node_id, node->id, edge_label, edge_color, edge_style);
        if (visited_count != node->visited) {
            break;
        }
        ++node->visited;

        write_cfg_node_decl(handle, node);

        write_cfg_node_to_dot_file(handle, visited_count, node->id, "", "", "", node->as.loop_entry->block.next);
    } break;
    case CFG_LOOP_EXIT_NODE: {
        fprintf(handle, "\tnode%ld -> node%ld [label=\"%s\", color=\"%s\", style=\"%s\"];\n", prev_node_id, node->id, edge_label, edge_color, edge_style);
        if (visited_count != node->visited) {
            break;
        }
        ++node->visited;

        write_cfg_node_decl(handle, node);

        write_cfg_node_to_dot_file(handle, visited_count, node->id, "", "", "", node->as.loop_exit->next);
    } break;
    case CFG_BACKEDGE_NODE: {
        fprintf(handle, "\tnode%ld -> node%ld [label=\"%s\", color=\"%s\", style=\"%s\"];\n", prev_node_id, node->id, edge_label, edge_color, edge_style);
        if (visited_count != node->visited) {
            break;
        }
        ++node->visited;

        write_cfg_node_decl(handle, node);

        write_cfg_node_to_dot_file(handle, visited_count, node->id, "", "", "dashed", node->as.backedge->next);
    } break;
    case CFG_BREAK_NODE: {
        fprintf(handle, "\tnode%ld -> node%ld [label=\"%s\", color=\"%s\", style=\"%s\"];\n", prev_node_id, node->id, edge_label, edge_color, edge_style);
        if (visited_count != node->visited) {
            break;
        }
        ++node->visited;

        write_cfg_node_decl(handle, node);

        write_cfg_node_to_dot_file(handle, visited_count, node->id, "", "", "dashed", node->as.break_->next);
    } break;
    default: {
        assert(false && "Unreachable");
    } break;
    };
}

bool write_cfg_dot_file(const char* filepath, CFGNode* cfg) {
    assert(filepath != NULL);

    FILE* file = NULL;
    i32 status = fopen_s(&file, filepath, "wb");
    if (status != 0 || file == NULL) {
        printf("Error: failed to open file \"%s\".\n", filepath);
        return false;
    }

    write_dot_header(file);

    write_cfg_node_to_dot_file(file, cfg->visited, 0, "", "", "", cfg);

    putc('}', file);

    fclose(file);

    printf("Saved CFG graph to \"%s\".\n", filepath);

    return true;
}