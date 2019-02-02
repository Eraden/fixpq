#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>
#include <stdio.h>

#include <types.h>
#include <parser.h>
#include <lexer.h>
#include <parser_test.h>

void test_parser_select_add(void **state) {
    Lexer *lexer = Lexer_init("./examples/select_add.psql");
    assert_non_null(lexer);

    Lexer_tokenize(lexer);

    Parser *parser = Parser_init(lexer);
    assert_non_null(parser);

    Parser_parse(parser);

    ParserToken *semicolon = parser->ast;
    assert_non_null(semicolon);
    assert_true(semicolon->type == ParserType_Semicolon);
    assert_non_null(semicolon->left);
    assert_null(semicolon->right);

    ParserToken *select = semicolon->left;
    assert_non_null(select);
    assert_true(select->type == ParserType_Select);
    assert_null(select->left);
    assert_non_null(select->right);

    ParserToken *add = select->left;
    assert_non_null(add);
    assert_true(add->type == ParserType_Add);
    assert_non_null(add->left);
    assert_non_null(add->right);

    ParserToken *one = add->left;
    assert_non_null(one);
    assert_true(one->type == ParserType_Number);
    assert_non_null(one->left);
    assert_non_null(one->right);

    ParserToken *two = add->right;
    assert_non_null(two);
    assert_true(two->type == ParserType_Number);
    assert_non_null(two->left);
    assert_non_null(two->right);

    Lexer_free(lexer);
    Parser_free(parser);
}
