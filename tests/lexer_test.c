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
    assert_true(lexer->tokens[0]->position.character == 1);
    assert_true(lexer->tokens[1]->type == LexerType_Keyword);
    assert_true(lexer->tokens[1]->position.character == 8);
    assert_true(lexer->tokens[2]->type == LexerType_Identifier);
    assert_true(lexer->tokens[2]->position.character == 18);
    assert_true(lexer->tokens[3]->type == LexerType_Separator); // ;
    assert_true(lexer->tokens[3]->position.character == 24); // ;

    Lexer_free(lexer);
}

void test_lexer_valid_select_star_from_table(void **state) {
    Lexer *lexer = Lexer_init("./examples/valid_select_star_from_table.psql");
    assert_non_null(lexer);

    Lexer_tokenize(lexer);
    assert_true(lexer->tokenLen == 5);
    assert_true(lexer->tokens[0]->type == LexerType_Keyword);
    assert_true(lexer->tokens[0]->position.character == 1);
    assert_true(lexer->tokens[1]->type == LexerType_Operator);
    assert_true(lexer->tokens[1]->position.character == 8);
    assert_true(lexer->tokens[2]->type == LexerType_Keyword);
    assert_true(lexer->tokens[2]->position.character == 10);
    assert_true(lexer->tokens[3]->type == LexerType_Identifier);
    assert_true(lexer->tokens[3]->position.character == 15);
    assert_true(lexer->tokens[4]->type == LexerType_Separator);
    assert_true(lexer->tokens[4]->position.character == 20);

    Lexer_free(lexer);

}
