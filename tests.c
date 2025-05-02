#include <assert.h>
#include <string.h>

#include "tests.h"
#include "str.h"
#include "map.h"
#include "str-list.h"
#include "str-list-inline.h"
#include "val-list.h"
#include "var-decl-list.h"
#include "stmt.h"
#include "log.h"

/* TODO: move these into their respective units */

static void test_strings(void)
{
    assert(toy_str_equal("same", "same") == TOY_TRUE);
    assert(toy_str_equal("not", "same") == TOY_FALSE);
    assert(toy_str_nequal("same", "same") == TOY_FALSE);
    assert(toy_str_nequal("not", "same") == TOY_TRUE);
}

static listitem_callback_result str_list_item_callback(void *cookie, size_t index, const toy_str_list *list)
{
    size_t *count = (size_t *) cookie;
    // log_printf("Got string '%s'\n", list->str);
    assert(*count == 0 || *count == 1);
    assert((*count != 0) || (toy_str_equal(list->str, "first string")));
    assert((*count != 1) || (toy_str_equal(list->str, "second string")));
    (*count)++;
    return CONTINUE_ITERATING;
}

static void test_str_list(void)
{
    toy_str first_str = "first string";
    toy_str_list *str_list = str_list_alloc(first_str);
    assert(toy_str_equal(str_list->str, first_str));
    assert(NULL == str_list->next);
    assert(1 == str_list_len(str_list));
    toy_str second_str = "second string";
    toy_str_list *retval = str_list_append(str_list, second_str);
    assert(toy_str_equal(retval->str, first_str));
    assert(NULL != retval->next);
    assert(toy_str_equal(retval->next->str, second_str));
    assert(NULL == retval->next->next);
    assert(2 == str_list_len(retval));
    size_t count = 0;
    str_list_foreach_const(str_list, str_list_item_callback, &count);
    str_list_free(retval);
}

static listitem_callback_result str_list_inline_item_callback(void *cookie, size_t index, const toy_str_list_inline *list)
{
    size_t *count = (size_t *) cookie;
    // log_printf("Got string '%s'\n", list->str);
    assert(*count == 0 || *count == 1);
    assert((*count != 0) || (toy_str_equal(str_list_inline_payload_const(list), "first string")));
    assert((*count != 1) || (toy_str_equal(str_list_inline_payload_const(list), "second string")));
    (*count)++;
    return CONTINUE_ITERATING;
}

static void test_str_list_inline(void)
{
    toy_str first_str = "first string";
    toy_str_list_inline *str_list = str_list_inline_alloc(first_str);
    assert(toy_str_equal(&str_list->c, first_str));
    assert(NULL == str_list->next);
    assert(1 == str_list_inline_len(str_list));
    toy_str second_str = "second string";
    toy_str_list_inline *retval = str_list_inline_append(str_list, second_str);
    assert(toy_str_equal(&retval->c, first_str));
    assert(NULL != retval->next);
    assert(toy_str_equal(&retval->next->c, second_str));
    assert(NULL == retval->next->next);
    assert(2 == str_list_inline_len(retval));
    size_t count = 0;
    str_list_inline_foreach_const(str_list, str_list_inline_item_callback, &count);
    str_list_inline_free(retval);
}

static void test_str_lists(void)
{
    test_str_list();
    test_str_list_inline();
}

static void test_val_lists(void)
{
    toy_val first_elem = { .type = VAL_BOOL, .boolean = TOY_TRUE };
    toy_val_list *val_list = val_list_alloc(&first_elem);
    assert(val_list);
    assert(val_list->val.type == VAL_BOOL);
    assert(val_list->val.boolean == TOY_TRUE);
    assert(val_list->next == NULL);
    assert(1 == val_list_len(val_list));
    toy_val new_elem = { .type = VAL_BOOL, .boolean = TOY_FALSE };
    toy_val_list *retval = val_list_append(val_list, &new_elem);
    assert(retval == val_list);
    assert(retval->next);
    assert(!retval->next->next);
    assert(retval->next->val.type == VAL_BOOL);
    assert(retval->next->val.boolean == TOY_FALSE);
    assert(2 == val_list_len(retval));
    val_list_free(retval);
}

static void test_maps(void)
{
    toy_map *map1 = map_alloc();

    assert(0 == map_len(map1));
    // Test first insert
    toy_val val1 = { .type = VAL_NUM, .num = 42 };
    map_set(map1, "first key", &val1);
    assert(1 == map_len(map1));
    toy_val *get1 = map_get(map1, "first key");
    assert(get1->type == VAL_NUM);
    assert(get1->num == 42);

    // Test second insert
    toy_val val2 = { .type = VAL_STR, .str = "second value" };
    map_set(map1, "second key", &val2);
    assert(2 == map_len(map1));
    toy_val *get2 = map_get(map1, "second key");
    assert(get2->type == VAL_STR);
    assert(0 == strcmp(get2->str, "second value"));
    toy_val *get3 = map_get(map1, "first key");
    assert(get3->type == VAL_NUM);
    assert(get3->num == 42);

    // Test delete
    map_delete(map1, "first key");
    assert(1 == map_len(map1));
    toy_val *get4 = map_get(map1, "first key");
    assert(NULL == get4);
    toy_val *get5 = map_get(map1, "second key");
    assert(get5->type == VAL_STR);
    assert(0 == strcmp(get5->str, "second value"));

    // Test overwrite
    toy_val new_value = { .type = VAL_STR, .str = "new value" };
    map_set(map1, "second key", &new_value);
    assert(1 == map_len(map1));
    toy_val *get6 = map_get(map1, "second key");
    assert(get6->type == VAL_STR);
    assert(0 == strcmp(get6->str, "new value"));

    // Test reset
    map_reset(map1);
    assert(0 == map_len(map1));

    // Test free
    map_free(map1);
}

static void test_var_decl_lists(void)
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
    var_decl_list_free(decl_list);
}

static void test_expr_lists()
{
    /* TODO */
}

void run_tests(void)
{
    test_strings();
    test_str_lists();
    test_val_lists();
    test_maps();
    test_var_decl_lists();
    test_expr_lists();
}
