#include <types.h>

Lexer *Lexer_init(char *file_path);
void Lexer_free(Lexer *tokenizer);
int Lexer_tokenize(Lexer *lexer);
