#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <lexer_test.h>
#include <parser_test.h>

int main(void) {
    const struct CMUnitTest tests[] = {
//            cmocka_unit_test(test_lexer_create_extension),
//            cmocka_unit_test(test_lexer_valid_select_star_from_table),
//            cmocka_unit_test(test_parser_select_add),
//            cmocka_unit_test(test_parser_syntax_error_table),
            cmocka_unit_test(test_parser_valid_select_star_from_table),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
