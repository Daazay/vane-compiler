#include "vanec/compiler/compiler_options.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "vanec/utils/string_utils.h"

void compiler_options_init(CompilerOptions* options) {
    if (options == NULL) {
        return;
    }
    options->command = COMPILER_COMMAND_UNDEFINED;
    options->flags = COMPILER_FLAG_NONE;

    options->files = vector_create(DEFAULT_VECTOR_CAPACITY, sizeof(char*), &str_free, true);
    options->stream_chunk_capacity = MIN_STREAM_CHUNK_CAPACITY;
    options->output_dir = NULL;
    options->working_dir = NULL;
}

typedef struct {
    CompilerOptions* options;

    u32 args_count;
    char** args;

    u32 arg_index;
    const char* current_arg;
} ArgParserContext;

#define PRINT(msg, ...) printf(msg "\n", ##__VA_ARGS__)
#define PRINT_ERR(msg, ...) printf("Error: " msg "\n", ##__VA_ARGS__)

static void print_usage(const char* path) {
    PRINT("Usage: %s [options] command [arguments]", path);
    PRINT("");
    PRINT("Commands:");
    PRINT("  lex <file> [<file> ...]   Lex a file into tokens.");
    PRINT("  ast <file> [<file> ...]   Parse a file into an abstract syntax tree.");
    PRINT("");
    PRINT("Options:");
    PRINT("  --output_dir <dir> - Specify the output dirpath.");
    PRINT("  --chunk_cap <number> - Specify stream chunk capacity.");
}

static inline bool is_option(const char* arg) {
    return arg[0] == '-';
}

static inline bool is_long_option(const char* arg) {
    return str_len(arg) > 2 && (arg[0] == '-' && arg[1] == '-');
}

static inline bool is_short_option(const char* arg) {
    return str_len(arg) == 2 && (arg[0] == '-' && arg[1] != '-');
}

static inline bool match_arg(const char* opt, const char* expected) {
    return str_eq(opt, expected);
}

static inline bool has_next(ArgParserContext* ctx) {
    return ctx->arg_index + 1 < ctx->args_count;
}

static inline bool is_next_opt(ArgParserContext* ctx) {
    return is_option(ctx->args[ctx->arg_index + 1]);
}

static inline bool parse_option(ArgParserContext* ctx) {
    if (is_long_option(ctx->current_arg)) {
        const char* opt = ctx->current_arg + 2;

        if (match_arg(opt, "output_dir")) {
            if (is_next_opt(ctx)) {
                PRINT_ERR("The argument for the \"output_dir\" option was not provided.");
                return false;
            }
            ctx->options->output_dir = str_dup(ctx->args[++ctx->arg_index]);
            return true;
        }
        else if (match_arg(opt, "chunk_cap")) {
            if (is_next_opt(ctx)) {
                PRINT_ERR("The argument for the \"chunk_cap\" option was not provided.");
                return false;
            }
            const char* number = ctx->args[++ctx->arg_index];
            u64 cap = atoi(number);

            ctx->options->stream_chunk_capacity = cap;

            return true;
        }
    }
    PRINT_ERR("Unknown option \"%s\".", ctx->current_arg);
    return false;
}

static inline void append_arg_files(ArgParserContext* ctx, Vector* vector) {
    while (has_next(ctx) && !is_next_opt(ctx)) {
        const char* arg = str_dup(ctx->args[++ctx->arg_index]);
        vector_push_back(vector, &arg);
    }
}

static inline bool parse_command(ArgParserContext* ctx) {
    if (match_arg(ctx->current_arg, "lex")) {
        ctx->options->command = COMPILER_COMMAND_LEX_ONLY;

        append_arg_files(ctx, &ctx->options->files);
        if (ctx->options->files.items_count == 0) {
            PRINT_ERR("The \"lex\" command need at least one file.");
            return false;
        }
        return true;
    }
    if (match_arg(ctx->current_arg, "ast")) {
        ctx->options->command = COMPILER_COMMAND_PARSE_AST_ONLY;

        append_arg_files(ctx, &ctx->options->files);
        if (ctx->options->files.items_count == 0) {
            PRINT_ERR("The \"ast\" command need at least one file.");
            return false;
        }
        return true;
    }
    PRINT_ERR("Unknown command \"%s\".", ctx->current_arg);
    return false;
}

bool compiler_options_parse_args(CompilerOptions* options, int argc, char** argv) {
    if (argc < 2) {
        print_usage(argv[0]);
        return false;
    }

    ArgParserContext ctx = {
        .options = options,
        .args_count = argc,
        .args = argv,
        .arg_index = 1,
        .current_arg = NULL,
    };

    for (ctx.arg_index = 1; ctx.arg_index < ctx.args_count; ++ctx.arg_index) {
        ctx.current_arg = ctx.args[ctx.arg_index];

        if (is_option(ctx.current_arg)) {
            if (!parse_option(&ctx)) {
                return false;
            }
            continue;
        }
        if (options->command == COMPILER_COMMAND_UNDEFINED) {
            if (!parse_command(&ctx)) {
                return false;
            }
            continue;
        }

        PRINT_ERR("Found an unexpected argument \"%s\".\n", ctx.current_arg);
        return false;
    }

    if (ctx.options->command == COMPILER_COMMAND_UNDEFINED) {
        PRINT_ERR("The command is not specified.\n");
        return false;
    }

    return true;
}

void compiler_options_free(CompilerOptions* options) {
    if (options == NULL) {
        return;
    }

    vector_free(&options->files);
    str_free(options->output_dir);
    str_free(options->working_dir);

    options->working_dir = NULL;
    options->output_dir = NULL;
}