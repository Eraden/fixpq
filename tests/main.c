#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <lexer_test.h>
#include <parser_test.h>

/* A test case that does nothing and succeeds. */
static void null_test_success(void **state) {
    (void) state; /* unused */
}

int main(void) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(null_test_success),
            cmocka_unit_test(test_lexer_create_extension),
            cmocka_unit_test(test_parser_select_add),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
