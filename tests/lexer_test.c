#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <cmocka.h>

#include <types.h>
#include <lexer.h>
#include <lexer_test.h>

void test_lexer_create_extension(void **state) {
    Lexer *lexer = Lexer_init("./examples/create_extension.psql");
    assert_non_null(lexer);

    Lexer_tokenize(lexer);
    assert_true(lexer->tokenLen == 4);
    assert_true(lexer->tokens[0]->type == LexerType_Keyword);
    assert_true(lexer->tokens[1]->type == LexerType_Keyword);
    assert_true(lexer->tokens[2]->type == LexerType_Identifier);
    assert_true(lexer->tokens[3]->type == LexerType_Operator); // ;
//    assert_true(lexer->tokens[4]->type == LexerType_Identifier); // \n

    Lexer_free(lexer);
}
