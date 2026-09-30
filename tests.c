#include <assert.h>
#include <string.h>

#include "tests.h"
#include "test-list.h"
#include "test-map-str-val.h"
#include "test-map-str-ptr.h"
#include "test-string.h"
#include "test-str-list.h"
#include "test-str-list-inline.h"
#include "test-val-list.h"
#include "test-var-decl-list.h"
#include "test-stmt-list.h"
#include "test-set-str.h"

static void test_expr_list()
{
    /* TODO */
}

void run_tests(void)
{
    test_string();
    test_list();
    test_str_list();
    test_str_list_inline();
    test_val_list();
    test_map_str_val();
    test_map_str_ptr();
    test_var_decl_list();
    test_expr_list();
    test_stmt_list();
    test_set_str();
}
