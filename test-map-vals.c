#include <assert.h>
#include <string.h>

#include "str.h"
#include "test-map-vals.h"

#include "val.h"
#include "stmt-list.h"
#include "generic-map.h"
#include "map-val.h"

typedef struct predicate_args_struct {
    toy_str intended_key;
    toy_val *intended_value;
} predicate_args;

static toy_bool test_map_entry(void *cookie, const toy_str key, const toy_val *value)
{
    predicate_args *args = (predicate_args *) cookie;
    assert(toy_str_equal(key, args->intended_key));
    assert_vals_equal(value, args->intended_value);
    return TOY_TRUE;
}

static item_callback_result map_item_callback(void *cookie, const map_val_entry *entry)
{
    if (!test_map_entry(cookie, entry->key, &entry->payload.value)) {
        return STOP_ENUMERATION;
    }
    return CONTINUE_ENUMERATION;
}

void test_map_vals(void)
{
    // Test create
    map_val *map1 = map_val_alloc();

    assert(0 == map_val_size(map1));
    // Test first insert
    toy_val val1 = { .type = VAL_NUM, .num = 42 };
    set_result val1_set_res = map_val_set(map1, "first key", &val1);
    assert(SET_NEW == val1_set_res);
    assert(1 == map_val_size(map1));

    // Test retrieve
    toy_val *get1 = map_val_get(map1, "first key");
    assert(get1->type == VAL_NUM);
    assert(get1->num == 42);

    // Test second insert
    toy_val val2 = { .type = VAL_STR, .str = "second value" };
    set_result val2_set_res = map_val_set(map1, "second key", &val2);
    assert(SET_NEW == val2_set_res);
    assert(2 == map_val_size(map1));
    toy_val *get2 = map_val_get(map1, "second key");
    assert(get2->type == VAL_STR);
    assert(toy_str_equal(get2->str, "second value"));
    toy_val *get3 = map_val_get(map1, "first key");
    assert(get3->type == VAL_NUM);
    assert(get3->num == 42);

    // Test delete
    map_val_delete(map1, "first key");
    assert(1 == map_val_size(map1));
    toy_val *get4 = map_val_get(map1, "first key");
    assert(NULL == get4);
    toy_val *get5 = map_val_get(map1, "second key");
    assert(get5->type == VAL_STR);
    assert(toy_str_equal(get5->str, "second value"));

    // Test overwrite
    toy_val val3 = { .type = VAL_STR, .str = "new value" };
    set_result val3_set_res = map_val_set(map1, "second key", &val3);
    assert(SET_EXISTING == val3_set_res);
    assert(1 == map_val_size(map1));
    toy_val *get6 = map_val_get(map1, "second key");
    assert(get6->type == VAL_STR);
    assert(toy_str_equal(get6->str, "new value"));

    // Test enumerate
    predicate_args args = { .intended_key = "second key", .intended_value = &val3};
    enumeration_result res = map_val_foreach_const(map1, map_item_callback, &args);
    assert(res == ENUMERATION_COMPLETE);

    // Test reset
    map_val_reset(map1);
    assert(0 == map_val_size(map1));

    // Test free
    map_val_free(map1);
}
