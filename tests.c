#include <assert.h>
#include <string.h>

#include "tests.h"
#include "map.h"

void test_maps(void)
{
    toy_map *map1 = alloc_map();
    toy_expr val1;
    val1.type = EXPR_NUM;
    val1.num = 42;
    map_set(map1, "first key", &val1);
    toy_expr *get1 = map_get(map1, "first key");
    assert(get1->type == EXPR_NUM);
    assert(get1->num == 42);
    toy_expr val2;
    val2.type = EXPR_STR;
    val2.str = "second value";
    map_set(map1, "second key", &val2);
    toy_expr *get2 = map_get(map1, "second key");
    assert(get2->type == EXPR_STR);
    assert(0 == strcmp(get2->str, "second value"));
    toy_expr *get3 = map_get(map1, "first key");
    assert(get3->type == EXPR_NUM);
    assert(get3->num == 42);
    dump_map(stderr, map1);
}

void run_tests(void)
{
    test_maps();
}
