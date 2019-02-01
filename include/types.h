#pragma once

typedef enum {
    FLAG_NoOp,
    FLAG_Input,
    FLAG_Output,
    FLAG_Help
} Flag;

typedef struct {
    char *input;
    char *output;
    enum Flag_e flag;
    FILE *in;
    FILE *out;
} State;

typedef enum {
    TokenizerType_Keyword,
    TokenizerType_Identifier,
    TokenizerType_Operator,
} TokenizerType;

typedef enum {
    LexerType_Keyword,
    LexerType_Identifier,
    LexerType_Operator,
} LexerType;

typedef struct {
    TokenizerType type;
    char *str;
} TokenizerToken;

typedef struct {
    LexerToken *left;
    LexerToken *right;
    LexerType type;
} LexerToken;

