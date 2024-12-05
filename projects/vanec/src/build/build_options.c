#include "vanec/build/build_options.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OUTPUT(string, ...) printf(string "\n", ##__VA_ARGS__)

#define EXIT_WITH_ERROR(options, string, ...)   \
do {                                            \
    printf(string "\n", ##__VA_ARGS__);         \
    print_usage();                              \
    vector_free(&options->files);               \
    exit(-1);                                   \
} while (false)

typedef struct {
    BuildOptions* options;

    u32 args_count;
    char** args;

    u32 arg_index;
    const char* current_arg;
} ArgParser;

void build_options_free(BuildOptions* options) {
    if (options == NULL) {
        return;
    }

    vector_free(&options->files);
}

static void print_usage() {
    OUTPUT("Usage: vanec [<options>] <command> [<args>]");
    OUTPUT("");
    OUTPUT("Commands:");
    OUTPUT("  lex <file> [<file> ...]   Lex a file into tokens.");
    OUTPUT("  ast <file> [<file> ...]   Parse a file into an abstract syntax tree.");
    OUTPUT("");
    OUTPUT("Options:");
    OUTPUT("  --out <file>   - specify the output filepath.");
    OUTPUT("  --print_tokens - Print tokens.");
}

bool is_command_accepts_files(const BuildCommand command) {
    switch (command) {
    case BUILD_COMMAND_LEX:         return true;
    case BUILD_COMMAND_PARSE_AST:   return true;
    case BUILD_COMMAND_MISSING:     return false;
    };
    return false;
}

static inline bool has_next(const ArgParser* parser) {
    assert(parser != NULL);

    return parser->arg_index < parser->args_count - 1;
}

static inline bool is_next_opt(ArgParser* parser) {
    assert(parser != NULL);

    return parser->args[parser->arg_index + 1][0] == '-';
}

static inline const char* get_next(ArgParser* parser) {
    assert(parser != NULL);

    return parser->args[++parser->arg_index];
}

static inline bool match_longopt(ArgParser* parser, const char* name) {
    assert(parser != NULL);

    return strcmp(parser->current_arg + 2, name) == 0;
}

static inline bool match_shortopt(ArgParser* parser, const char* name) {
    assert(parser != NULL);

    return strcmp(parser->current_arg + 1, name) == 0;
}

static inline bool match_command(ArgParser* parser, const char* name) {
    assert(parser != NULL);

    return strcmp(parser->current_arg, name) == 0;
}

static inline void parse_option(ArgParser* parser) {
    assert(parser != NULL);

    const u64 len = strlen(parser->current_arg);

    if (len < 3) {
        EXIT_WITH_ERROR(parser->options, "error: invalid option \"%s\".", parser->current_arg);
    }

    if (match_longopt(parser, "print_tokens")) {
        parser->options->print_tokens = true;
        return;
    }

    EXIT_WITH_ERROR(parser->options, "error: unknown option \"%s\".", parser->current_arg);
}

static inline void parse_command(ArgParser* parser) {
    assert(parser != NULL);

    if (match_command(parser, "lex")) {
        parser->options->command = BUILD_COMMAND_LEX;

        while (has_next(parser) && !is_next_opt(parser)) {
            parser->current_arg = get_next(parser);
            vector_push_back(&parser->options->files, &parser->current_arg);
        }

        if (parser->options->files.items_count == 0) {
            EXIT_WITH_ERROR(parser->options, "error: \"lex\" command needs at least one file.");
        }
        return;
    }
    else if (match_command(parser, "ast")) {
        parser->options->command = BUILD_COMMAND_PARSE_AST;
        
        while (has_next(parser) && !is_next_opt(parser)) {
            vector_push_back(&parser->options->files, &parser->current_arg);
            parser->current_arg = get_next(parser);
        }

        if (parser->options->files.items_count == 0) {
            EXIT_WITH_ERROR(parser->options, "error: \"ast\" command needs at least one file.");
        }
        return;
    }
    EXIT_WITH_ERROR(parser->options, "error: cannot process the unknown command \"%s\".", parser->current_arg);
}


static inline void append_file(ArgParser* parser) {
    assert(parser != NULL);
}

void parse_arguments(BuildOptions* options, int argc, char** argv) {
    assert(options != NULL);

    if (argc < 2) {
        print_usage();
        return;
    }

    ArgParser parser = {
        .options = options,
        .args_count = argc - 1,
        .args = (argc == 1) ? NULL : argv + 1,
        .arg_index = 0,
        .current_arg = NULL,
    };

    *options = (BuildOptions) {
        .command = BUILD_COMMAND_MISSING,
        .files = vector_create(DEFAULT_VECTOR_CAPACITY, sizeof(char*), NULL, true),
    };

    for (parser.arg_index = 0; parser.arg_index < parser.args_count; ++parser.arg_index) {
        parser.current_arg = parser.args[parser.arg_index];

        if (parser.current_arg[0] == '-') {
            parse_option(&parser);
            continue;
        }
        else if (parser.options->command == BUILD_COMMAND_MISSING) {
            parse_command(&parser);
            continue;
        }
        EXIT_WITH_ERROR(parser.options, "Found the unexpected argument \"%s\".", parser.current_arg);
    }
    if (parser.options->command == BUILD_COMMAND_MISSING) {
        EXIT_WITH_ERROR(parser.options, "No command found.");
    }
}