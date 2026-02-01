#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "test-lists.h"
#include "str.h"
#include "generic-list.h"
#include "str-list.h"
#include "log.h"
#include "list-filter.h"
#include "list-visitor.h"
#include "list-latch.h"

static void assert_all_match(const generic_list *list, generic_list_filter_func filter_func)
{
    assert(list_all_match(list, filter_func, NULL));
}

static void assert_none_match(const generic_list *list, generic_list_filter_func filter_func)
{
    assert(list_none_match(list, filter_func, NULL));
}

static void assert_not_all_match(const generic_list *list, generic_list_filter_func filter_func)
{
    assert(list_not_all_match(list, filter_func, NULL));
}

static void assert_some_match(const generic_list *list, generic_list_filter_func filter_func)
{
    assert(list_some_match(list, filter_func, NULL));
}

static toy_bool matches_all_test_data_1(void *cookie, size_t index, const toy_str_list *item)
{
    assert(!cookie);
    toy_str str = str_list_payload_const(item);
    return str_contains_nocase(str, "item");
}

static toy_bool matches_none_test_data_1(void *cookie, size_t index, const toy_str_list *item)
{
    assert(!cookie);
    toy_str str = str_list_payload_const(item);
    return str_contains_nocase(str, "this should not be present and contains utter garbage like shinfandoodle");
}

static toy_bool matches_proper_subset_test_data_1(void *cookie, size_t index, const toy_str_list *item)
{
    assert(!cookie);
    toy_str str = str_list_payload_const(item);
    return str_contains_nocase(str, "Third") || str_contains_nocase(str, "Fourth");
}

static void test_set_algebra(toy_str_list *list)
{
    list_assert_all_match((const generic_list *) list, (generic_list_filter_func) matches_all_test_data_1, NULL);
    list_assert_none_match((const generic_list *) list, (generic_list_filter_func) matches_none_test_data_1, NULL);
    list_assert_not_all_match((const generic_list *) list, (generic_list_filter_func) matches_proper_subset_test_data_1, NULL);
    list_assert_some_match((const generic_list *) list, (generic_list_filter_func) matches_proper_subset_test_data_1, NULL);
}

typedef struct array_compare_visitor_struct {
    list_visitor visitor;
    const toy_str *str_array;
    toy_bool result;
} array_compare_visitor;

static item_callback_result array_compare_visit_entry(array_compare_visitor *compare_visitor, size_t index, const toy_str_list *item)
{
    const toy_str list_str = str_list_payload_const(item);
    const toy_str array_str = compare_visitor->str_array[index];
    if (!str_equal(list_str, array_str)) {
        compare_visitor->result = TOY_FALSE;
        return STOP_ENUMERATION;
    }
    return CONTINUE_ENUMERATION;
}

static toy_bool str_list_equals_array(const toy_str_list *str_list, const toy_str *str_array)
{
    array_compare_visitor array_compare = {
        .visitor.visit_entry = (list_entry_visit_func) array_compare_visit_entry,
        .str_array = str_array,
        .result = TOY_TRUE
    };
    enumeration_result res = list_visitor_visit_list((list_visitor *) &array_compare, (generic_list *) str_list);
    assert(ENUMERATION_COMPLETE == res || ENUMERATION_INTERRUPTED == res);
    return array_compare.result;
}

static toy_bool is_edible(void *cookie, size_t index, const toy_str_list *item)
{
    const toy_str str = str_list_payload_const(item);
    if (
        str_equal(str, "Apples")
        || str_equal(str, "Oranges")
        || str_equal(str, "Watermelon")
        || str_equal(str, "Bread")
        || str_equal(str, "Rice")
    ) {
        return TOY_TRUE;
    }
    if (
        str_equal(str, "Concrete")
    ) {
        return TOY_FALSE;
    }
    assert(0);
}

static toy_bool is_fruit(void *cookie, size_t index, const toy_str_list *item)
{
    const toy_str str = str_list_payload_const(item);
    if (
        str_equal(str, "Apples")
        || str_equal(str, "Oranges")
        || str_equal(str, "Watermelon")
    ) {
        return TOY_TRUE;
    }
    if (
        str_equal(str, "Concrete")
        || str_equal(str, "Bread")
        || str_equal(str, "Rice")
    ) {
        return TOY_FALSE;
    }
    assert(0);
}

static void test_visitors(void)
{
    toy_str_list *list = str_list_alloc("Apples");
    list = str_list_append(list, "Oranges");
    const toy_str expected_values_1[] = { "Apples", "Oranges" };
    assert(str_list_equals_array(list, expected_values_1));
    list = str_list_append(list, "Watermelon");
    const toy_str expected_values_2[] = { "Apples", "Oranges", "Watermelon" };
    assert(str_list_equals_array(list, expected_values_2));
    assert_all_match((generic_list *) list, (generic_list_filter_func) match_always);
    assert_none_match((generic_list *) list, (generic_list_filter_func) match_never);
    assert_all_match((generic_list *) list, (generic_list_filter_func) is_edible);
    assert_all_match((generic_list *) list, (generic_list_filter_func) is_fruit);
    list = str_list_append(list, "Bread");
    list = str_list_append(list, "Rice");
    assert_all_match((generic_list *) list, (generic_list_filter_func) match_always);
    assert_none_match((generic_list *) list, (generic_list_filter_func) match_never);
    assert_all_match((generic_list *) list, (generic_list_filter_func) is_edible);
    assert_some_match((generic_list *) list, (generic_list_filter_func) is_fruit);
    assert_not_all_match((generic_list *) list, (generic_list_filter_func) is_fruit);
    list = str_list_append(list, "Concrete");
    assert_all_match((generic_list *) list, (generic_list_filter_func) match_always);
    assert_none_match((generic_list *) list, (generic_list_filter_func) match_never);
    assert_some_match((generic_list *) list, (generic_list_filter_func) is_edible);
    assert_not_all_match((generic_list *) list, (generic_list_filter_func) is_edible);
    assert_some_match((generic_list *) list, (generic_list_filter_func) is_fruit);
    assert_not_all_match((generic_list *) list, (generic_list_filter_func) is_fruit);
    /* TODO: Remove list elements */
    list = (toy_str_list *) list_delete_filter((generic_list *) list, (generic_list_filter_func) is_edible, NULL, TOY_FALSE, TOY_FALSE, (list_entry_free_func) str_list_free);
}

void test_lists(void)
{
    toy_str_list *list = str_list_alloc("First item");
    list = str_list_append(list, "Second item");
    list = str_list_append(list, "Third item");
    list = str_list_append(list, "Fourth item");
    list = str_list_append(list, "Fifth item");
    test_set_algebra(list);
    str_list_free(list);
    test_visitors();
}
