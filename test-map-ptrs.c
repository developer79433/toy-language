#include <assert.h>
#include <string.h>

#include "str.h"
#include "test-map-ptrs.h"
#include "map-ptr.h"
#include "val.h"
#include "util.h"

typedef struct my_filter_args_struct {
    toy_str intended_key;
    toy_val *intended_value;
} my_filter_args;

static toy_bool test_map_entry(void *cookie, toy_str key, const toy_val *value)
{
    my_filter_args *args = (my_filter_args *) cookie;
    assert(toy_str_equal(key, args->intended_key));
    assert(value != NULL);
    assert(args->intended_value != NULL);
    assert_vals_equal(value, args->intended_value);
    assert(value == args->intended_value);
    return TOY_TRUE;
}

static item_callback_result map_item_callback(void *cookie, const map_ptr_entry *entry)
{
    const toy_val *value = (toy_val *) entry->ptr;
    val_assert_valid(value);
    if (!test_map_entry(cookie, entry->key, value)) {
        return STOP_ENUMERATION;
    }
    return CONTINUE_ENUMERATION;
}

/* TODO: Belongs elsewhere */
void assert_ptr_array_one_not_null(void **arr, size_t len)
{
    const void *found_ptr = ptr_array_get_not_null(arr, len);
    assert(found_ptr != NULL);
}

void assert_one_bucket_not_null(map_ptr_entry_list **arr, size_t len)
{
    assert_ptr_array_one_not_null((void **) arr, len);
}

void test_map_ptrs(void)
{
    // Test create
    map_ptr *map1 = map_ptr_alloc();
    assert(0 == map_ptr_size(map1));

    // Test first insert
    toy_val val1 = { .type = VAL_NUM, .num = 42 };
    set_result val1_set_res = map_ptr_set(map1, "first key", &val1);
    assert(SET_NEW == val1_set_res);
    assert(1 == map_ptr_size(map1));
    assert(map1->num_items == 1);
    assert(map1->buckets != NULL);
    assert_one_bucket_not_null(map1->buckets, ELEMENTSOF(map1->buckets));
    map_ptr_entry_list *bucket = ptr_array_get_not_null((void **) map1->buckets, ELEMENTSOF(map1->buckets));
    assert(bucket != NULL);
    assert(bucket->next == NULL);
    assert(toy_str_equal(bucket->entry.key, "first key"));
    assert(bucket->entry.ptr != NULL);
    assert(bucket->entry.ptr == &val1);

    // Test retrieve
    toy_val *get1 = map_ptr_get(map1, "first key");
    assert(get1 == &val1);
    assert(get1->type == VAL_NUM);
    assert(get1->num == 42);

    // Test second insert
    toy_val val2 = { .type = VAL_STR, .str = "second value" };
    set_result val2_set_res = map_ptr_set(map1, "second key", &val2);
    assert(SET_NEW == val2_set_res);
    assert(2 == map_ptr_size(map1));
    toy_val *get2 = map_ptr_get(map1, "second key");
    assert(get2 == &val2);
    assert(get2->type == VAL_STR);
    assert(toy_str_equal(get2->str, "second value"));
    toy_val *get3 = map_ptr_get(map1, "first key");
    assert(get3->type == VAL_NUM);
    assert(get3->num == 42);

    // Test delete
    delete_result delete1_res = map_ptr_delete(map1, "first key");
    assert(delete1_res == DELETED);
    assert(1 == map_ptr_size(map1));
    toy_val *get4 = map_ptr_get(map1, "first key");
    assert(NULL == get4);
    toy_val *get5 = map_ptr_get(map1, "second key");
    assert(get5 == &val2);
    assert(get5->type == VAL_STR);
    assert(toy_str_equal(get5->str, "second value"));

    // Test overwrite
    toy_val val3 = { .type = VAL_STR, .str = "new value" };
    set_result val3_set_res = map_ptr_set(map1, "second key", &val3);
    assert(SET_EXISTING == val3_set_res);
    assert(1 == map_ptr_size(map1));
    toy_val *get6 = map_ptr_get(map1, "second key");
    assert(get6 == &val3);
    assert(get6->type == VAL_STR);
    assert(toy_str_equal(get6->str, "new value"));

    // Test enumerate
    my_filter_args args = { .intended_key = "second key", .intended_value = &val3};
    enumeration_result res = map_ptr_foreach_const(map1, map_item_callback, &args);
    assert(res == ENUMERATION_COMPLETE);

    // Test reset
    map_ptr_reset(map1);
    assert(0 == map_ptr_size(map1));

    // Test free
    map_ptr_free(map1);
}
