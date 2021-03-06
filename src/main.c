#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <types.h>
#include <simple.h>
#include <lexer.h>
#include <parser.h>

static const char *HELP_MSG = ""
                              "fixpq - remove invalid for PostgreSQL 9.6 parts in sql dumb\n"
                              "  -h | --help       show this message\n"
                              "  -o | --out=file   write to target file\n"
                              "  -f | --file=file  read from file\n";
static const char *SHORT_HELP_FLAG = "-h";
static const char *LONG_HELP_FLAG = "--help";
static const char *SHORT_INPUT_FLAG = "-f";
static const char *LONG_INPUT_FLAG = "--file";
static const char *SHORT_OUTPUT_FLAG = "-o";
static const char *LONG_OUTPUT_FLAG = "--out";
static const char *LONG_DRY_FLAG = "--dry";

void print_help(int status) {
    printf("%s\n", HELP_MSG);
    exit(status);
}

void copy_to(char **dest, char *src) {
    if (src == NULL) return;
    size_t len = strlen(src) + 1;
    *dest = (char *) malloc(len);
    memset((void *) *dest, 0, len);
    strcpy(*dest, src);
}

void open_in(State *state) {
    if (!state->input) {
        return;
    }
    state->in = fopen(state->input, "r");
    if (state->in == NULL) {
        printf("File not found: %s\n", state->input);
        exit(1);
    }
}


void parse_opts(int argc, char **argv, State *state) {
    for (int i = 0; i < argc; i++) {
        char *value = argv[i];

        switch (state->flag) {
            case FLAG_Output: {
                copy_to(&state->output, value);
                state->flag = FLAG_NoOp;
                break;
            }
            case FLAG_Input: {
                copy_to(&state->input, value);
                state->flag = FLAG_NoOp;
                break;
            }
            case FLAG_NoOp: {
                if (strcmp(value, SHORT_INPUT_FLAG) == 0) {
                    state->flag = FLAG_Input;
                } else if (strcmp(value, SHORT_OUTPUT_FLAG) == 0) {
                    state->flag = FLAG_Output;
                } else if (strcmp(value, SHORT_HELP_FLAG) == 0) {
                    print_help(0);
                } else if (strcmp(value, LONG_HELP_FLAG) == 0) {
                    print_help(0);
                } else if (strstr(value, LONG_INPUT_FLAG) == value) {
                    copy_to(&state->input, value + strlen(LONG_INPUT_FLAG) + 1);
                } else if (strstr(value, LONG_OUTPUT_FLAG) == value) {
                    copy_to(&state->output, value + strlen(LONG_OUTPUT_FLAG) + 1);
                } else if (strcmp(value, LONG_DRY_FLAG) == 0) {
                    state->dry = 1;
                }
                break;
            }
            case FLAG_Help: {
                break;
            }
        }
    }
}

int main(int argc, char **argv) {
    State *state = (State *) malloc(sizeof(State));
    state->output = NULL;
    state->input = NULL;
    state->flag = FLAG_NoOp;
    state->in = NULL;
    state->out = NULL;

    parse_opts(argc, argv, state);

    if (state->input == NULL || strlen(state->input) == 0) {
        print_help(1);
    } else if (state->output == NULL || strlen(state->output) == 0) {
        copy_to(&state->output, state->input);
    }

    open_in(state);
    fix_content(state);

    Lexer *tokenizer = Lexer_init(state->input);
    Lexer_tokenize(tokenizer);

    Parser *parser = Parser_init(tokenizer);
    Parser_parse(parser);
    if (parser->ast) {
        size_t count = 0;
        ParserToken *token = parser->ast;
        while (token != NULL) {
            count += 1;
            if (token->right)
                count += 1;
            token = token->left;
        }
        printf("Tree size: %zu\n", count);
    }

    if (!Parser_is_ok(parser)) {
        switch (parser->error) {
            case ParserError_Valid:
                break;
            case ParserError_AllocFailed:
                perror("Failed to allocate memory\n");
                break;
            case ParserError_InvalidTableParent:
                perror("Invalid `TABLE` parent\n");
                break;
        }
    }

    Parser_free(parser);
    Lexer_free(tokenizer);

    printf("Input: %s\nOutput: %s\n", state->input, state->output);

    if (state->input) free(state->input);
    if (state->output) free(state->output);

    if (state->in) fclose(state->in);
    if (state->out) fclose(state->out);

    return 0;
}
