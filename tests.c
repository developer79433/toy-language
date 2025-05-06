#include <assert.h>
#include <string.h>

#include "tests.h"
#include "test-map-vals.h"
#include "test-map-ptrs.h"
#include "test-strings.h"
#include "test-str-list.h"
#include "test-str-list-inline.h"
#include "test-val-lists.h"
#include "test-var-decl-lists.h"
#include "test-stmt-lists.h"

static void test_expr_lists()
{
    /* TODO */
}

void run_tests(void)
{
    test_strings();
    test_str_lists();
    test_str_list_inlines();
    test_val_lists();
    test_map_vals();
    test_map_ptrs();
    test_var_decl_lists();
    test_expr_lists();
    test_stmt_lists();
}
