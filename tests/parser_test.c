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

/**
 *               ;
 *           /       \
 *        SELECT
 *       /      \
 *       +
 *    /     \
 *   1        2
 *
 * */
void test_parser_select_add(void **state) {
    Lexer *lexer = Lexer_init("./examples/select_add.psql");
    assert_non_null(lexer);

    Lexer_tokenize(lexer);

    Parser *parser = Parser_init(lexer);
    assert_non_null(parser);

    Parser_parse(parser);

    assert_true(Parser_is_ok(parser) == 0);
    assert_true(parser->error == ParserError_Valid);

    ParserToken *semicolon = parser->ast;
    assert_non_null(semicolon);
    assert_true(semicolon->type == ParserType_Semicolon);
    assert_non_null(semicolon->left);
    assert_null(semicolon->right);

    ParserToken *select = semicolon->left;
    assert_non_null(select);
    assert_true(select->type == ParserType_Select);
    assert_non_null(select->left);
    assert_null(select->right);

    ParserToken *add = select->left;
    assert_non_null(add);
    assert_true(add->type == ParserType_Add);
    assert_non_null(add->left);
    assert_non_null(add->right);

    ParserToken *two = add->right;
    assert_non_null(two);
    assert_true(two->type == ParserType_Number);
    assert_null(two->left);
    assert_null(two->right);
    assert_string_equal(two->str, L"2");

    ParserToken *one = add->left;
    assert_non_null(one);
    assert_true(one->type == ParserType_Number);
    assert_null(one->left);
    assert_null(one->right);
    assert_string_equal(one->str, L"1");

    Lexer_free(lexer);
    Parser_free(parser);
}

void test_parser_syntax_error_table(void **state) {
    Lexer *lexer = Lexer_init("./examples/syntax_error_table.psql");
    assert_non_null(lexer);

    Lexer_tokenize(lexer);
    Parser *parser = Parser_init(lexer);
    assert_non_null(parser);
    Parser_parse(parser);

    assert_true(Parser_is_ok(parser) == 0);
    assert_true(parser->error == ParserError_InvalidTableParent);

    Lexer_free(lexer);
    Parser_free(parser);
}

void test_parser_valid_select_star_from_table(void **state) {
    Lexer *lexer = Lexer_init("./examples/valid_select_star_from_table.psql");
    assert_non_null(lexer);
    Lexer_tokenize(lexer);
    Parser *parser = Parser_init(lexer);
    assert_non_null(parser);
    Parser_parse(parser);

    assert_true(Parser_is_ok(parser) == 0);
    assert_true(parser->error == ParserError_Valid);

    ParserToken *semicolon = parser->ast;
    assert_non_null(semicolon);
    assert_true(semicolon->type == ParserType_Semicolon);
    assert_non_null(semicolon->left);
    assert_null(semicolon->right);

    ParserToken *from = semicolon->left;
    assert_non_null(from);
    assert_true(from->type == ParserType_From);
    assert_non_null(from->left);
    assert_non_null(from->right);

    ParserToken *table = from->right;
    assert_non_null(table);
    assert_true(table->type == ParserType_Identifier);
    assert_null(table->left);
    assert_null(table->right);
    assert_string_equal(table->str, L"users");

    ParserToken *select = from->left;
    assert_non_null(select);
    assert_true(select->type == ParserType_Select);
    assert_null(select->left);
    assert_non_null(select->right);

    ParserToken *star = select->left;
    assert_non_null(star);
    assert_true(star->type == ParserType_Star);
    assert_null(star->left);
    assert_null(star->right);
}
