#include <assert.h>
#include <string.h>

#include "tests.h"
#include "toy-str.h"
#include "toy-map.h"
#include "toy-val-list.h"

static void test_strings(void)
{
    assert(toy_str_equal("same", "same") == TOY_TRUE);
    assert(toy_str_equal("not", "same") == TOY_FALSE);
}

static void test_val_lists(void)
{
    toy_val first_elem = { .type = VAL_BOOL, .boolean = TOY_TRUE };
    toy_val_list *val_list = alloc_val_list(&first_elem);
    assert(val_list);
    assert(val_list->val->type == VAL_BOOL);
    assert(val_list->val->boolean == TOY_TRUE);
    assert(val_list->next == NULL);
    toy_val new_elem = { .type = VAL_BOOL, .boolean = TOY_FALSE };
    toy_val_list *retval = append_val_list_own(val_list, &new_elem);
    assert(retval == val_list);
    assert(val_list->next);
    assert(!val_list->next->next);
    assert(val_list->next->val);
    assert(val_list->next->val->type == VAL_BOOL);
    assert(val_list->next->val->boolean == TOY_FALSE);
}

static void test_maps(void)
{
    toy_map *map1 = alloc_map();

    // Test first insert
    toy_val val1 = { .type = VAL_NUM, .num = 42 };
    map_set(map1, "first key", &val1);
    toy_val *get1 = map_get(map1, "first key");
    assert(get1->type == VAL_NUM);
    assert(get1->num == 42);

    // Test second insert
    toy_val val2 = { .type = VAL_STR, .str = "second value" };
    map_set(map1, "second key", &val2);
    toy_val *get2 = map_get(map1, "second key");
    assert(get2->type == VAL_STR);
    assert(0 == strcmp(get2->str, "second value"));
    toy_val *get3 = map_get(map1, "first key");
    assert(get3->type == VAL_NUM);
    assert(get3->num == 42);

    // Test delete
    map_delete(map1, "first key");
    toy_val *get4 = map_get(map1, "first key");
    assert(NULL == get4);
    toy_val *get5 = map_get(map1, "second key");
    assert(get5->type == VAL_STR);
    assert(0 == strcmp(get5->str, "second value"));

    // Test overwrite
    toy_val new_value = { .type = VAL_STR, .str = "new value" };
    map_set(map1, "second key", &new_value);
    toy_val *get6 = map_get(map1, "second key");
    assert(get6->type == VAL_STR);
    assert(0 == strcmp(get6->str, "new value"));
}

void run_tests(void)
{
    test_strings();
    test_maps();
    test_val_lists();
}
