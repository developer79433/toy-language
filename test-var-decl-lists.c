#include <assert.h>

#include "test-var-decl-lists.h"
#include "var-decl-list.h"

void test_var_decl_lists(void)
{
    toy_expr expr1 = { .type = EXPR_LITERAL, .val.type = VAL_BOOL, .val.boolean = TOY_TRUE };
    char *name1 = "name1";
    toy_var_decl decl1 = { .name = name1, .value = &expr1 };
    toy_var_decl_list *decl_list = var_decl_list_alloc(&decl1);
    assert(decl_list->decl.name == name1);
    assert(decl_list->decl.value == &expr1);
    assert(decl_list->next == NULL);
    toy_expr expr2 = { .type = EXPR_LITERAL, .val.type = VAL_STR, .val.str = "value2" };
    char *name2 = "name2";
    toy_var_decl decl2 = { .name = name2, .value = &expr2 };
    decl_list = var_decl_list_append(decl_list, &decl2);
    assert(decl_list->decl.name == name1);
    assert(decl_list->decl.value == &expr1);
    assert(decl_list->next != NULL);
    assert(decl_list->next->decl.name == name2);
    assert(decl_list->next->decl.value == &expr2);
    assert(decl_list->next->next == NULL);
    /* TODO: Test enumeration */
    var_decl_list_free(decl_list);
}
