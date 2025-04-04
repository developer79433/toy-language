#include <assert.h>
#include <string.h>

#include "tests.h"
#include "map.h"

void test_maps(void)
{
    toy_map *map1 = alloc_map();

    // Test first insert
    toy_expr val1 = { .type = EXPR_LITERAL, .val.type = VAL_NUM, .val.num = 42 };
    map_set(map1, "first key", &val1);
    toy_expr *get1 = map_get(map1, "first key");
    assert(get1->type == EXPR_LITERAL);
    assert(get1->val.type == VAL_NUM);
    assert(get1->val.num == 42);

    // Test second insert
    toy_expr val2 = { .type = EXPR_LITERAL, .val.type = VAL_STR, .val.str = "second value" };
    map_set(map1, "second key", &val2);
    toy_expr *get2 = map_get(map1, "second key");
    assert(get2->type == EXPR_LITERAL);
    assert(get2->val.type == VAL_STR);
    assert(0 == strcmp(get2->val.str, "second value"));
    toy_expr *get3 = map_get(map1, "first key");
    assert(get3->type == EXPR_LITERAL);
    assert(get3->val.type == VAL_NUM);
    assert(get3->val.num == 42);

    // Test delete
    map_delete(map1, "first key");
    toy_expr *get4 = map_get(map1, "first key");
    assert(NULL == get4);
    toy_expr *get5 = map_get(map1, "second key");
    assert(get5->type == EXPR_LITERAL);
    assert(get5->val.type == VAL_STR);
    assert(0 == strcmp(get5->val.str, "second value"));

    // Test overwrite
    toy_expr new_value = { .type = EXPR_LITERAL, .val.type = VAL_STR, .val.str = "new value" };
    map_set(map1, "second key", &new_value);
    toy_expr *get6 = map_get(map1, "second key");
    assert(get6->type == EXPR_LITERAL);
    assert(get6->val.type == VAL_STR);
    assert(0 == strcmp(get6->val.str, "new value"));
}

void run_tests(void)
{
    test_maps();
}
