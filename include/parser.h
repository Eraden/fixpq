#include <types.h>

Parser *Parser_init(Lexer *tokenizer);
void Parser_free(Parser *parser);
int Parser_parse(Parser *parser);
