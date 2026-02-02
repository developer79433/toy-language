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
#include "rev-list-visitor.h"
#include "rev-list-subset-visitor.h"

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

#if 0
static item_callback_result print_entry(rev_list_visitor *rev_list_vis, size_t index, toy_str_list *item)
{
    toy_str str = str_list_payload(item);
    log_debug("Backwards: Item %zu is %s\n", index, str);
    return CONTINUE_ENUMERATION;
}
#endif

typedef struct rev_list_compare_visitor_struct {
    rev_list_subset_visitor rev_list_sub_vis;
    toy_str_list *other_list;
} rev_list_compare_visitor;

void rev_list_compare_visitor_init(rev_list_compare_visitor *rev_list_comp_vis, list_entry_visit_func visit_func, generic_list *stop_at, toy_str_list *other_list)
{
    rev_list_subset_visitor *rev_list_stop_vis = &rev_list_comp_vis->rev_list_sub_vis;
    rev_list_subset_visitor_init(rev_list_stop_vis, visit_func, stop_at);
    rev_list_comp_vis->other_list = other_list;
}

static item_callback_result compare_to_other_list(rev_list_compare_visitor *rev_list_comp_vis, size_t index, toy_str_list *my_item)
{
    toy_str my_str = str_list_payload(my_item);
    toy_str other_str = str_list_index(rev_list_comp_vis->other_list, index);
    assert(str_equal(my_str, other_str));
    return CONTINUE_ENUMERATION;
}

static item_callback_result test_entry_valid(list_visitor *list, size_t index, toy_str_list *item)
{
    assert(index >= 0 && index < 1000000);
    str_list_assert_valid(item);
    toy_str str = str_list_payload(item);
    str_assert_valid(str);
    return CONTINUE_ENUMERATION;
}

static void test_backwards_visitors()
{
    toy_str_list *list = str_list_alloc("First item");
    list = str_list_append(list, "Second item");
    list = str_list_append(list, "Third item");
    list = str_list_append(list, "Fourth item");
    list = str_list_append(list, "Fifth item");

    toy_str_list *reversed = str_list_alloc("Fifth item");
    reversed = str_list_append(reversed, "Fourth item");
    reversed = str_list_append(reversed, "Third item");
    reversed = str_list_append(reversed, "Second item");
    reversed = str_list_append(reversed, "First item");
    rev_list_compare_visitor rev_list_comp_vis_1;
    rev_list_compare_visitor_init(&rev_list_comp_vis_1, (list_entry_visit_func) compare_to_other_list, NULL, list);
    rev_list_subset_visitor_visit_list((rev_list_subset_visitor *) &rev_list_comp_vis_1, (generic_list *) list);

    rev_list_visitor rev_list_vis_1;
    rev_list_visitor_init(&rev_list_vis_1, (list_entry_visit_func) test_entry_valid );
    rev_list_visitor_visit_list(&rev_list_vis_1, (generic_list *) list);

    rev_list_subset_visitor rev_list_vis_2;
    rev_list_subset_visitor_init(&rev_list_vis_2, (list_entry_visit_func) test_entry_valid, (generic_list *) list->next->next->next);
    rev_list_subset_visitor_visit_list(&rev_list_vis_2, (generic_list *) list);

    toy_str_list *shorter = str_list_alloc("First item");
    shorter = str_list_append(shorter, "Second item");
    shorter = str_list_append(shorter, "Third item");
    shorter = str_list_append(shorter, "Fourth item");
    rev_list_compare_visitor rev_list_comp_vis_2;
    rev_list_compare_visitor_init(&rev_list_comp_vis_2, (list_entry_visit_func) compare_to_other_list, (generic_list *) list->next->next->next, shorter);
    rev_list_subset_visitor_visit_list((rev_list_subset_visitor *) &rev_list_comp_vis_2, (generic_list *) list);
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
    /* TODO: Test result */
}

void test_lists(void)
{
    toy_str_list *list = str_list_alloc("First item");
    list = str_list_append(list, "Second item");
    list = str_list_append(list, "Third item");
    list = str_list_append(list, "Fourth item");
    list = str_list_append(list, "Fifth item");
    test_backwards_visitors();
    test_set_algebra(list);
    str_list_free(list);
    test_visitors();
}
