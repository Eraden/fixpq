#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <wchar.h>
#include <locale.h>

typedef struct {
    size_t line;
    size_t character;
    size_t position;
} FilePosition;

typedef enum {
    FLAG_NoOp,
    FLAG_Input,
    FLAG_Output,
    FLAG_Help
} Flag;

typedef struct {
    char *input;
    char *output;
    Flag flag;
    FILE *in;
    FILE *out;
} State;

typedef enum {
    LexerType_Keyword,
    LexerType_Identifier,
    LexerType_Operator,
    LexerType_Literal,
} LexerType;

typedef enum {
    ParserType_Number,
    ParserType_Identifier,
    ParserType_Smaller,
    ParserType_SmallerOrEqual,
    ParserType_Highter,
    ParserType_HighterOrEqual,
    ParserType_Equal,
    ParserType_Assign,
    ParserType_Add,
    ParserType_Substitute,
    ParserType_Multiply,
    ParserType_Divide,
    ParserType_Modulo,
    ParserType_Semicolon,
    ParserType_BinaryOr,
    ParserType_BinaryAnd,
    ParserType_Create,
    ParserType_Alter,
    ParserType_Drop,
    ParserType_Table,
    ParserType_Function,
    ParserType_Extension,
} ParserType;

typedef struct {
    LexerType type;
    wchar_t *str;
    FilePosition position;
} LexerToken;

typedef struct ParserToken_t {
    struct ParserToken_t *left;
    struct ParserToken_t *right;
    wchar_t *str;
    ParserType type;
    FilePosition position;
} ParserToken;

typedef struct {
    FILE *in;
    FilePosition position;
    LexerToken **tokens;
    size_t tokenLen;
    wchar_t *buffer;
} Lexer;

typedef struct {
    LexerToken **tokens;
    ParserToken *ast;
    size_t tokenLen;
    size_t position;
} Parser;
