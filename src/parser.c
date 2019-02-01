#include <parser.h>
#include <ctype.h>

void Parser_free_token(ParserToken *token);

static short is_ok(Parser *parser);

static ParserToken *ParserToken_new(LexerToken *lexerToken);

static ParserToken *consume(Parser *parser);

// Matchers
static short is_number(const wchar_t *str);
static short is_operator(const wchar_t *str);

// Consume lexer tokens
static ParserToken *consume_lexer_keyword(Parser *parser, LexerToken *lexerToken);
static ParserToken *consume_lexer_identifier(Parser *parser, LexerToken *current);
static ParserToken *consume_lexer_operator(Parser *parser, LexerToken *current);
static ParserToken *consume_lexer_literal(Parser *parser, LexerToken *current);

// Consume parser tokens
static ParserToken *consume_table_token(Parser *parser, ParserToken *current);
static ParserToken *consume_function_token(Parser *parser, ParserToken *current);
static ParserToken *consume_extension_token(Parser *parser, ParserToken *current);


// Implementations
Parser *Parser_init(Lexer *tokenizer) {
    Parser *parser = (Parser *) malloc(sizeof(Parser));
    memset(parser, 0, sizeof(Parser));
    parser->tokens = tokenizer->tokens;
    parser->tokenLen = tokenizer->tokenLen;
    return parser;
}

void Parser_free(Parser *parser) {
    if (parser->ast) Parser_free_token(parser->ast);
    free(parser);
}

void Parser_free_token(ParserToken *token) {
    if (token->left) Parser_free_token(token->left);
    if (token->right) Parser_free_token(token->right);
    if (token->str) free(token->str);
    free(token);
}

int Parser_parse(Parser *parser) {
    while (is_ok(parser)) {
        parser->ast = consume(parser);
    }
    return 1;
}

static ParserToken *ParserToken_new(LexerToken *lexerToken) {
    ParserToken *token = (ParserToken *) malloc(sizeof(ParserToken));
    memset(token, 0, sizeof(ParserToken));
    token->type = ParserType_Create;
    token->position.line = lexerToken->position.line;
    token->position.character = lexerToken->position.character;
    token->position.position = lexerToken->position.position;
    return token;
}

static ParserToken *consume(Parser *parser) {
    LexerToken *current = parser->tokens[parser->position];
    ParserToken *token = parser->ast;
    if (current == NULL)
        return token;

    switch (current->type) {
        case LexerType_Keyword:
            token = consume_lexer_keyword(parser, current);
            break;
        case LexerType_Identifier:
            token = consume_lexer_identifier(parser, current);
            break;
        case LexerType_Operator:
            token = consume_lexer_operator(parser, current);
            break;
        case LexerType_Literal:
            token = consume_lexer_literal(parser, current);
            break;
    }

    parser->position += 1;
    return token;
}

static ParserToken *consume_lexer_keyword(Parser *parser, LexerToken *lexerToken) {
    ParserToken *token = parser->ast;
    ParserToken *root = parser->ast;

    if (wcscmp(lexerToken->str, L"CREATE") == 0) {
        token = ParserToken_new(lexerToken);
        token->type = ParserType_Create;
        token->left = root;
        parser->ast = token;
    } else if (wcscmp(lexerToken->str, L"ALTER") == 0) {
        token = ParserToken_new(lexerToken);
        token->type = ParserType_Alter;
        token->left = root;
        parser->ast = token;
    } else if (wcscmp(lexerToken->str, L"DROP") == 0) {
        token = ParserToken_new(lexerToken);
        token->type = ParserType_Drop;
        token->left = root;
        parser->ast = token;
    } else if (wcscmp(lexerToken->str, L"TABLE") == 0) {
        ParserToken *current = ParserToken_new(lexerToken);
        current->type = ParserType_Table;
        token = consume_table_token(parser, current);
    } else if (wcscmp(lexerToken->str, L"FUNCTION") == 0) {
        ParserToken *current = ParserToken_new(lexerToken);
        current->type = ParserType_Function;
        token = consume_function_token(parser, current);
    } else if (wcscmp(lexerToken->str, L"EXTENSION") == 0) {
        ParserToken *current = ParserToken_new(lexerToken);
        current->type = ParserType_Extension;
        token = consume_extension_token(parser, current);
    }
    return token;
}

static ParserToken *consume_lexer_identifier(Parser *parser, LexerToken *lexerToken) {
    ParserToken *token = parser->ast;
    return token;
}

static ParserToken *consume_lexer_operator(Parser *parser, LexerToken *lexerToken) {
    ParserToken *token = parser->ast;
    return token;
}

static ParserToken *consume_lexer_literal(Parser *parser, LexerToken *lexerToken) {
    ParserToken *root = parser->ast;
    ParserToken *token = ParserToken_new(lexerToken);
    parser->ast = token;

    if (is_operator(lexerToken->str)) {
        switch (*lexerToken->str) {
            case L'=':
                if (root != NULL && root->type == ParserType_Assign) {
                    root->type = ParserType_Equal;
                    Parser_free_token(token);
                    token = root;
                } else {
                    token->type = ParserType_Assign;
                }
                break;
            case L'+':
                token->type = ParserType_Add;
                break;
            case L'-':
                token->type = ParserType_Substitute;
                break;
            case L'*':
                token->type = ParserType_Multiply;
                break;
            case L'/':
                token->type = ParserType_Divide;
                break;
            case L'%':
                token->type = ParserType_Modulo;
                break;
            case L'|':
                token->type = ParserType_BinaryOr;
                break;
            case L'&':
                token->type = ParserType_BinaryAnd;
                break;
            case L';':
                token->type = ParserType_Semicolon;
                break;
            default:
                break;
        }
    } else if (is_number(lexerToken->str)) {
        token->type = ParserType_Number;
    }
    if (token != root)
        token->left = root;
    return token;
}

static short is_ok(Parser *parser) {
    return parser->position < parser->tokenLen;
}

static ParserToken *consume_table_token(Parser *parser, ParserToken *current) {
    if (parser->ast == NULL)
        return current;
    ParserToken *root = parser->ast;
    parser->ast = current;
    switch (root->type) {
        case ParserType_Drop:
        case ParserType_Alter:
        case ParserType_Create: {
            root->right = current;
            return root;
        }
        default: {
            current->left = root;
        }
    }
    return current;
}

static ParserToken *consume_function_token(Parser *parser, ParserToken *current) {
    if (parser->ast == NULL)
        return current;
    ParserToken *root = parser->ast;
    parser->ast = current;
    switch (root->type) {
        case ParserType_Drop:
        case ParserType_Alter:
        case ParserType_Create: {
            root->right = current;
            return root;
        }
        default: {
            current->left = root;
        }
    }
    return current;
}

static ParserToken *consume_extension_token(Parser *parser, ParserToken *current) {
    if (parser->ast == NULL)
        return current;
    ParserToken *root = parser->ast;
    parser->ast = current;
    switch (root->type) {
        case ParserType_Drop:
        case ParserType_Alter:
        case ParserType_Create: {
            root->right = current;
            return root;
        }
        default: {
            current->left = root;
        }
    }
    return current;
}

// MATCHERS
static short is_number(const wchar_t *b) {
    if (b == NULL) return 0;
    size_t len = wcslen(b);
    short hadDot = 0;
    for (size_t i = 0; i < len; i++) {
        wchar_t wc = b[i];
        char c = (char) wc;
        if (!isdigit(c) && wc != L'.') {
            return 0;
        } else if (wc == L'.' && hadDot == 0) {
            hadDot = 1;
        } else if (wc == L'.' && hadDot == 1) {
            return 0;
        }
    }
    return 1;
}

static short is_operator(const wchar_t *b) {
    if (b == NULL) return 0;
    size_t len = wcslen(b);
    if (len > 1) return 0;
    if (len < 1) return 0;
    switch (*b) {
        case L'=':
        case L'+':
        case L'-':
        case L'*':
        case L'/':
        case L'%':
        case L'|':
        case L'&':
        case L';':
            return 1;
        default:
            return 0;
    }
}
