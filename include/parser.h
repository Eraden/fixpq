#include <types.h>

Parser *Parser_init(Lexer *lexer);
void Parser_free(Parser *parser);
int Parser_parse(Parser *parser);
