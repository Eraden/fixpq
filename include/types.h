#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <wchar.h>
#include <locale.h>
#include <ctype.h>

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
    ParserError_AllocFailed,
} ParserError;

typedef enum {
    LexerType_Keyword,
    LexerType_Identifier,
    LexerType_Operator,
    LexerType_Literal,
} LexerType;

typedef enum {
    ParserType_InlineComment,
    ParserType_MultiLineComment,
    ParserType_Number,
    ParserType_Identifier,
    ParserType_Smaller,
    ParserType_Dot,
    ParserType_SmallerOrEqual,
    ParserType_Larger,
    ParserType_LargerOrEqual,
    ParserType_Equal,
    ParserType_Assign,
    ParserType_Add,
    ParserType_Subtraction,
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
    ParserType_Select,
    ParserType_LeftParenthesis,
    ParserType_RightParenthesis,
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
    LexerToken *lexerToken;
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
    ParserError error;
} Parser;
