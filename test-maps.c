#include <assert.h>
#include <string.h>

#include "str.h"
#include "test-maps.h"

#include "val.h"
#include "map.h"

typedef struct predicate_args_struct {
    toy_str intended_key;
    toy_val *intended_value;
} predicate_args;

static toy_bool test_map_entry(void *cookie, toy_str key, toy_val *value)
{
    predicate_args *args = (predicate_args *) cookie;
    assert(toy_str_equal(key, args->intended_key));
    assert(vals_equal(value, args->intended_value));
    return 1;
}

static item_callback_result map_item_callback(void *cookie, toy_str key, toy_val *value)
{
    if (!test_map_entry(cookie, key, value)) {
        return STOP_ENUMERATION;
    }
    return CONTINUE_ENUMERATION;
}

void test_maps(void)
{
    toy_map *map1 = generic_map_alloc();

    assert(0 == map_len(map1));
    // Test first insert
    toy_val val1 = { .type = VAL_NUM, .num = 42 };
    map_val_set(map1, "first key", &val1);
    assert(1 == map_len(map1));
    toy_val *get1 = map_val_get(map1, "first key");
    assert(get1->type == VAL_NUM);
    assert(get1->num == 42);

    // Test second insert
    toy_val val2 = { .type = VAL_STR, .str = "second value" };
    map_val_set(map1, "second key", &val2);
    assert(2 == map_len(map1));
    toy_val *get2 = map_val_get(map1, "second key");
    assert(get2->type == VAL_STR);
    assert(0 == strcmp(get2->str, "second value"));
    toy_val *get3 = map_val_get(map1, "first key");
    assert(get3->type == VAL_NUM);
    assert(get3->num == 42);

    // Test delete
    generic_map_delete(map1, "first key");
    assert(1 == map_len(map1));
    toy_val *get4 = map_val_get(map1, "first key");
    assert(NULL == get4);
    toy_val *get5 = map_val_get(map1, "second key");
    assert(get5->type == VAL_STR);
    assert(0 == strcmp(get5->str, "second value"));

    // Test overwrite
    toy_val new_value = { .type = VAL_STR, .str = "new value" };
    map_val_set(map1, "second key", &new_value);
    assert(1 == map_len(map1));
    toy_val *get6 = map_val_get(map1, "second key");
    assert(get6->type == VAL_STR);
    assert(0 == strcmp(get6->str, "new value"));

    // Test enumerate
    predicate_args args = { .intended_key = "second key", .intended_value = &new_value};
    enumeration_result res = generic_map_foreach(map1, map_item_callback, &args);
    assert(res == ENUMERATION_COMPLETE);

    // Test reset
    generic_map_reset(map1);
    assert(0 == map_len(map1));

    // Test free
    generic_map_free(map1);
}
