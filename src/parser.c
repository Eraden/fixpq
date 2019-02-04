#include <parser.h>

void Parser_free_token(ParserToken *token);

static ParserToken *ParserToken_new(LexerToken *lexerToken);

static ParserToken *consume(Parser *parser);

// Utils

static void store_str(Parser *parser, ParserToken *token, wchar_t *str, short sep);

static void parse_error(Parser *parser, ParserError error);

static ParserToken *first_keyword_in_tree(ParserToken *root, short stop_on_semicolon);

// Matchers
static short is_number(const wchar_t *b);

static short peek_n(const Parser *parser, size_t n, LexerToken const *lexerTokens[]);

static short is_inline_comment(const Parser *parser);

static short next_is_lexer_type(const Parser *parser, LexerType lexerType);

// Consume lexer tokens
static ParserToken *consume_lexer_keyword(Parser *parser, LexerToken *lexerToken);

static ParserToken *consume_lexer_identifier(Parser *parser, LexerToken *lexerToken);

static ParserToken *consume_lexer_operator(Parser *parser, LexerToken *lexerToken);

static ParserToken *consume_lexer_separator(Parser *parser, LexerToken *lexerToken);

static ParserToken *consume_lexer_literal(Parser *parser, LexerToken *lexerToken);

// Consume parser tokens
static ParserToken *consume_table_token(Parser *parser, ParserToken *current);

static ParserToken *consume_function_token(Parser *parser, ParserToken *token);

static ParserToken *consume_extension_token(Parser *parser, ParserToken *token);

static ParserToken *consume_inline_comment(Parser *parser, ParserToken *token);

static ParserToken *consume_subtraction(Parser *parser, ParserToken *token);

static ParserToken *consume_divide(Parser *parser, ParserToken *token);

static ParserToken *consume_assign(Parser *parser, ParserToken *token);

// Implementations
Parser *Parser_init(Lexer *lexer) {
    Parser *parser = (Parser *) malloc(sizeof(Parser));
    memset(parser, 0, sizeof(Parser));
    parser->tokens = lexer->tokens;
    parser->tokenLen = lexer->tokenLen;
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
    while (Parser_is_ok(parser)) {
        parser->ast = consume(parser);
    }
    return 1;
}

static ParserToken *ParserToken_new(LexerToken *lexerToken) {
    ParserToken *token = (ParserToken *) malloc(sizeof(ParserToken));
    memset(token, 0, sizeof(ParserToken));
    token->lexerToken = lexerToken;
    token->type = ParserType_Create;
    token->position.line = lexerToken->position.line;
    token->position.character = lexerToken->position.character;
    token->position.position = lexerToken->position.position;
    return token;
}

short Parser_is_ok(Parser *parser) {
    if (parser->error != ParserError_Valid)
        return 0;
    return parser->position < parser->tokenLen;
}

static ParserToken *consume(Parser *parser) {
    if (parser->position >= parser->tokenLen)
        return NULL;

    LexerToken *current = parser->tokens[parser->position];
    ParserToken *root = parser->ast;
    if (current == NULL)
        return root;

    switch (current->type) {
        case LexerType_Keyword:
            root = consume_lexer_keyword(parser, current);
            break;
        case LexerType_Identifier:
            root = consume_lexer_identifier(parser, current);
            break;
        case LexerType_Operator:
            root = consume_lexer_operator(parser, current);
            break;
        case LexerType_Literal:
            root = consume_lexer_literal(parser, current);
            break;
        case LexerType_Separator:
            root = consume_lexer_separator(parser, current);
            break;
    }

    parser->position += 1;
    return root;
}

static ParserToken *consume_lexer_keyword(Parser *parser, LexerToken *lexerToken) {
    ParserToken *token = NULL;
    ParserToken *root = parser->ast;

    if (wcscmp(lexerToken->str, L"SELECT") == 0) {
        token = ParserToken_new(lexerToken);
        token->type = ParserType_Select;
        token->left = root;
        parser->ast = token;
    } else if (wcscmp(lexerToken->str, L"CREATE") == 0) {
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
    } else if (wcscmp(lexerToken->str, L"FROM") == 0) {
        token = ParserToken_new(lexerToken);
        token->type = ParserType_Select;
        token->left = root;
        parser->ast = token;
    } else {
        token = root;
    }
    return token;
}

static ParserToken *consume_lexer_identifier(Parser *parser, LexerToken *lexerToken) {
    ParserToken *root = parser->ast;
    ParserToken *token = ParserToken_new(lexerToken);
    token->type = ParserType_Identifier;
    store_str(parser, token, lexerToken->str, 0);

    if (!root) {
        token->left = root;
        parser->ast = token;
    } else {
        switch (root->type) {
            case ParserType_Select: {
                root->right = token;
                parser->ast = token;
                return root;
            }
            case ParserType_From: {
                root->right = token;
                parser->ast = token;
                return root;
            }
            default: {
                token->left = root;
                parser->ast = token;
                break;
            }
        }
    }
    return token;
}

static ParserToken *consume_lexer_operator(Parser *parser, LexerToken *lexerToken) {
    ParserToken *root = parser->ast;
    ParserToken *token = ParserToken_new(lexerToken);
    token->left = root;
    parser->ast = NULL;

    switch (*lexerToken->str) {
        case L'=':
            return consume_assign(parser, token);
        case L'+':
            token->type = ParserType_Add;
            break;
        case L'-':
            return consume_subtraction(parser, token);
        case L'*': {
            ParserToken *keyword = first_keyword_in_tree(root, 1);
            if (keyword != NULL && keyword->type == ParserType_Select) {
                token->type = ParserType_Star;
                return token;
            } else {
                token->type = ParserType_Multiply;
            }
            break;
        }
        case L'/':
            return consume_divide(parser, token);
        case L'%':
            token->type = ParserType_Modulo;
            break;
        case L'|':
            token->type = ParserType_BinaryOr;
            break;
        case L'&':
            token->type = ParserType_BinaryAnd;
            break;
        default:
            break;
    }
    parser->position += 1;
    token->right = consume(parser);
    parser->position -= 1;
    return token;
}

static ParserToken *consume_lexer_separator(Parser *parser, LexerToken *lexerToken) {
    ParserToken *root = parser->ast;
    ParserToken *token = ParserToken_new(lexerToken);
    token->left = root;
    parser->ast = NULL;

    switch (*lexerToken->str) {
        case L';':
            token->type = ParserType_Semicolon;
            break;
        case L'<':
            token->type = ParserType_Smaller;
            break;
        case L'>':
            token->type = ParserType_Larger;
            break;
        case L'(':
            token->type = ParserType_LeftParenthesis;
            break;
        case L')':
            token->type = ParserType_RightParenthesis;
            break;
        case L'.':
            token->type = ParserType_Dot;
            break;
        case L',':
            token->type = ParserType_Comma;
            break;
        default:
            break;
    }
    parser->position += 1;
    token->right = consume(parser);
    parser->position -= 1;
    return token;
}

static ParserToken *consume_assign(Parser *parser, ParserToken *token) {
    ParserToken *root = token->left;

    if (root == NULL) {
        token->type = ParserType_Assign;
        return token;
    }

    switch (root->type) {
        case ParserType_Assign: {
            root->type = ParserType_Equal;
            Parser_free_token(token);
            token = root;
            break;
        }
        case ParserType_Larger: {
            root->type = ParserType_LargerOrEqual;
            Parser_free_token(token);
            token = root;
            break;
        }
        case ParserType_Smaller: {
            root->type = ParserType_SmallerOrEqual;
            Parser_free_token(token);
            token = root;
            break;
        }
        default:
            token->type = ParserType_Assign;
            break;
    }
    return token;
}

static ParserToken *consume_divide(Parser *parser, ParserToken *token) {
    token->type = ParserType_Divide;
    return token;
}

static ParserToken *consume_subtraction(Parser *parser, ParserToken *token) {
    ParserType type = ParserType_Subtraction;
    short is_comment = is_inline_comment(parser);
    if (is_comment) {
        type = ParserType_InlineComment;
        consume_inline_comment(parser, token);
    }

    token->type = type;
    return token;
}

static ParserToken *consume_inline_comment(Parser *parser, ParserToken *token) {
    parser->position += 1; // skip second `-`
    parser->position += 1;
    const size_t line = token->position.line;

    LexerToken **head = parser->tokens + parser->position;
    LexerToken **it = parser->tokens + parser->position;
    LexerToken **tail = NULL;
    LexerToken **last_in_line = NULL;

    while (Parser_is_ok(parser)) {
        LexerToken *current = *it;

        if (current == NULL)
            break;

        if (current->position.line != line) {
            parser->position -= 1;
            break;
        } else {
            it = it + 1;
            parser->position += 1;
        }
    }

    if (head != NULL && it != NULL && head != it) {
        tail = it;
        it = head;
        last_in_line = tail - 1;
        size_t from = (*head)->position.character;
        size_t last_in_line_len = (*last_in_line)->str ? wcslen((*last_in_line)->str) : 1;
        size_t line_len = (*(tail - 1))->position.character - (*head)->position.character + last_in_line_len;
        token->str = malloc(sizeof(wchar_t) * (line_len + 1));
        for (size_t i = 0; i < line_len; i++) token->str[i] = L' ';
        token->str[line_len] = 0;

        size_t size = 0;
        for (; it != tail;) {
            LexerToken *c = *it;
            wchar_t *dest = token->str;
            wchar_t *src = c->str;
            size_t pad = c->position.character - from;
            size_t len = src ? wcslen(src) : 0;
            if (len && size + len > line_len) {
                printf("Out of bound memory write!");
                exit(9);
            } else {
                size += len;
            }
            if (src) {
                wcsncpy(dest + pad, src, len);
            }
            it = it + 1;
        }
    }

    return token;
}

static ParserToken *consume_lexer_literal(Parser *parser, LexerToken *lexerToken) {
    ParserToken *root = parser->ast;
    ParserToken *token = ParserToken_new(lexerToken);
    parser->ast = token;
    store_str(parser, token, lexerToken->str, 0);

    if (is_number(lexerToken->str)) {
        token->type = ParserType_Number;
    }
    if (next_is_lexer_type(parser, LexerType_Operator)) {
        parser->position += 1;
        ParserToken *op = consume(parser);
        parser->position -= 1;
        if (root) {
            root->left = op;
            parser->ast = root;
            return root;
        } else {
            parser->ast = op;
            return op;
        }
    }
    token->left = root;
    return token;
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
            parse_error(parser, ParserError_InvalidTableParent);
            break;
        }
    }
    return current;
}

static ParserToken *consume_function_token(Parser *parser, ParserToken *token) {
    if (parser->ast == NULL)
        return token;
    ParserToken *root = parser->ast;
    parser->ast = token;
    switch (root->type) {
        case ParserType_Drop:
        case ParserType_Alter:
        case ParserType_Create: {
            root->right = token;
            return root;
        }
        default: {
            token->left = root;
        }
    }
    return token;
}

static ParserToken *consume_extension_token(Parser *parser, ParserToken *token) {
    if (parser->ast == NULL)
        return token;
    ParserToken *root = parser->ast;
    parser->ast = token;
    switch (root->type) {
        case ParserType_Drop:
        case ParserType_Alter:
        case ParserType_Create: {
            root->right = token;
            return root;
        }
        default: {
            token->left = root;
        }
    }
    return token;
}

// Utils
static void store_str(Parser *parser, ParserToken *token, wchar_t *str, short sep) {
    if (str == NULL && token->str == NULL)
        return;

    const size_t given_len = str ? wcslen(str) : 0;
    size_t old_len = token->str ? wcslen(token->str) : 0;

    if (token->str == NULL) {
        token->str = (wchar_t *) malloc(sizeof(wchar_t) * (given_len + 1));
        memset(token->str, 0, given_len + 1);
        if (str) wcscpy(token->str, str);
    } else if (sep || str != NULL) {
        size_t len = old_len + given_len + (sep ? 1 : 0) + 1;
        wchar_t *new_block = (wchar_t *) realloc(token->str, sizeof(wchar_t) * len);
        if (new_block) {
            token->str = new_block;
        } else {
            parser->position = parser->tokenLen;
            parser->error = ParserError_AllocFailed;
        }
        for (size_t i = old_len; i < len; i++)
            token->str[i] = 0;
        if (sep) {
            wcscat(token->str, L" ");
        }
        if (str != NULL)
            wcscat(token->str, str);
        token->str[len - 1] = 0;
    }
    token->position.character += 1;
}

static void parse_error(Parser *parser, ParserError error) {
    parser->error = error;
    parser->position = parser->tokenLen;
}

static ParserToken *first_keyword_in_tree(ParserToken *root, short stop_on_semicolon) {
    if (root == NULL)
        return NULL;
    if (root->lexerToken->type == LexerType_Keyword)
        return root;
    if (stop_on_semicolon && wcscmp(root->str, L";") == 0)
        return NULL;
    ParserToken *current = first_keyword_in_tree(root->right, stop_on_semicolon);
    if (current) return current;
    current = first_keyword_in_tree(root->left, stop_on_semicolon);
    return current;
}

// Matchers
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
        } else if (wc == L'.') {
            return 0;
        }
    }
    return 1;
}

static short peek_n(const Parser *parser, size_t n, LexerToken const *lexerTokens[]) {
    if (parser->tokenLen <= parser->position + n)
        return 0;
    for (size_t i = 0; i < n; i++) {
        lexerTokens[i] = parser->tokens[parser->position + i];
    }
    return 1;
}

static short is_inline_comment(const Parser *parser) {
    LexerToken const *lexerTokens[1];
    if (peek_n(parser, 1, lexerTokens) == 0)
        return 0;
    return lexerTokens[0]->type == LexerType_Operator && wcscmp(lexerTokens[0]->str, L"-") == 0;
}

static short next_is_lexer_type(const Parser *parser, LexerType lexerType) {
    if (parser->position + 1 >= parser->tokenLen)
        return 0;
    LexerToken *lexerToken = parser->tokens[parser->position + 1];
    if (lexerToken == NULL)
        return 0;
    return lexerToken->type == lexerType;
}
