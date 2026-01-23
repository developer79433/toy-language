#include <assert.h>
#include <string.h>

#include "util.h"
#include "str.h"
#include "test-map-ptrs.h"
#include "map-ptr.h"
#include "val.h"
#include "map-visitor.h"
#include "log.h"
#include "generic-map.h"

typedef struct my_filter_args_struct {
    toy_str intended_key;
    toy_val *intended_value;
} my_filter_args;

static toy_bool test_map_entry(void *cookie, toy_str key, const toy_val *value)
{
    my_filter_args *args = (my_filter_args *) cookie;
    assert(str_equal(key, args->intended_key));
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

void test_map_ptr_basics(void)
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
    assert(str_equal(bucket->entry.key, "first key"));
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
    assert(str_equal(get2->str, "second value"));
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
    assert(str_equal(get5->str, "second value"));

    // Test overwrite
    toy_val val3 = { .type = VAL_STR, .str = "new value" };
    set_result val3_set_res = map_ptr_set(map1, "second key", &val3);
    assert(SET_EXISTING == val3_set_res);
    assert(1 == map_ptr_size(map1));
    toy_val *get6 = map_ptr_get(map1, "second key");
    assert(get6 == &val3);
    assert(get6->type == VAL_STR);
    assert(str_equal(get6->str, "new value"));

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

typedef struct test_tuple_struct {
    toy_str key;
    toy_val value;
} test_tuple;

static test_tuple test_data[] = {
    { .key = "one", .value.type = VAL_NUM, .value.num = 1 },
    { .key = "two", .value.type = VAL_NUM, .value.num = 2 },
    { .key = "three", .value.type = VAL_NUM, .value.num = 3 },
    { .key = "four", .value.type = VAL_NUM, .value.num = 4 }
};

static void insert_test_data(map_ptr *map)
{
    for (test_tuple *tuple = &test_data[0]; tuple < &test_data[ELEMENTSOF(test_data)]; tuple++) {
        set_result set_res = map_ptr_set(map, tuple->key, &tuple->value);
        assert(SET_NEW == set_res);
    }
}

static item_callback_result print_entry(map_visitor *visitor, map_ptr_entry *entry)
{
    toy_val *val = (toy_val *) entry->ptr;
    log_printf("Map entry: { \"%s\" => ", entry->key);
    val_dump(val, TOY_FALSE);
    log_printf(" }\n");
    return CONTINUE_ENUMERATION;
}

static void test_visitors(void)
{
    map_ptr *map1 = map_ptr_alloc();
    assert(0 == map_ptr_size(map1));

    insert_test_data(map1);
    assert(ELEMENTSOF(test_data) == map_ptr_size(map1));
    map_visitor printer = { .visit_map = NULL, .visit_entry = (map_entry_visit_func) print_entry };
    enumeration_result res = map_visitor_visit_map(&printer, (generic_map *) map1);
    assert(ENUMERATION_COMPLETE == res);
}

static toy_bool compare_value(void *cookie, map_ptr_entry *entry)
{
    toy_val *val_to_find = (toy_val *) cookie;
    toy_val *this_val = (toy_val *) entry->ptr;
    return vals_equal(this_val, val_to_find);
}

static void test_find(void)
{
    map_ptr *map1 = map_ptr_alloc();
    assert(0 == map_ptr_size(map1));

    insert_test_data(map1);
    assert(ELEMENTSOF(test_data) == map_ptr_size(map1));

    toy_val val_to_find = { .type = VAL_NUM, .num = 3 };
    /* TODO: Create a ptr_map wrapper */
    map_ptr_entry *found = (map_ptr_entry *) generic_map_find_first((generic_map *) map1, (generic_map_filter_func) compare_value, &val_to_find);
    assert(found);
    assert(!strcmp(found->key, "three"));
    toy_val *val = (toy_val *) found->ptr;
    assert(VAL_NUM == val->type);
    assert(3 == val->num);
    log_printf("Found: { \"%s\" => %f }\n", found->key, val->num);
}

void test_map_ptrs(void)
{
    test_map_ptr_basics();
    test_visitors();
    test_find();
}
