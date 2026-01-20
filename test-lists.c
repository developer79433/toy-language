#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "test-lists.h"
#include "str.h"
#include "generic-list.h"
#include "str-list.h"
#include "log.h"

struct my_list_filter_struct;
typedef struct my_list_filter_struct my_list_filter;
struct my_list_filter_struct {
#ifdef INLINE_BASE
    list_visit_func visit_list;
    list_entry_visit_func visit_entry;
#else /* ndef INLINE_BASE */
    list_visitor base;
#endif /* INLINE_BASE */
    generic_list_filter_func filter_func;
    void *filter_cookie;
    toy_bool inverted;
    list_visitor *next_visitor;
};

item_callback_result list_filter_visit_entry(my_list_filter *filter, size_t index, generic_list *entry)
{
    assert((void *) filter == (void *) &filter->base);
    toy_bool filter_res = filter->filter_func(filter->filter_cookie, index, entry);
    if (filter->inverted) {
        filter_res = !filter_res;
    }
    if (filter_res) {
        list_visitor *next = filter->next_visitor;
        assert(next);
        return list_visitor_visit_entry(next, index, entry);
    }
    return CONTINUE_ENUMERATION;
}

enumeration_result list_filter_visit_list(my_list_filter *filter, generic_list *list)
{
    return list_visitor_visit_list(&filter->base, list);
}

void list_filter_init(my_list_filter *filter, generic_list_filter_func filter_func, void *filter_cookie, toy_bool inverted, list_visitor *next_visitor)
{
    filter->base.visit_list = NULL;
    filter->base.visit_entry = (list_entry_visit_func) list_filter_visit_entry;
    filter->filter_func = filter_func;
    filter->filter_cookie = filter_cookie;
    filter->inverted = inverted;
    filter->next_visitor = next_visitor;
}

typedef struct latch_visitor_struct {
    list_visitor base;
    generic_list *last_seen_item;
    toy_bool stop_on_first;
} latch_visitor;

item_callback_result latch_visitor_visit_entry(latch_visitor *latch, size_t index, generic_list *item)
{
    latch->last_seen_item = item;
    return latch->stop_on_first ? STOP_ENUMERATION : CONTINUE_ENUMERATION;
}

void latch_visitor_init(latch_visitor *latch, toy_bool stop_on_first)
{
    latch->base.visit_list = NULL;
    latch->base.visit_entry = (list_entry_visit_func) latch_visitor_visit_entry;
    latch->last_seen_item = NULL;
    latch->stop_on_first = stop_on_first;
}

generic_list *list_find(generic_list *list, generic_list_filter_func filter_func, void *filter_cookie, toy_bool stop_on_first, toy_bool inverted)
{
    latch_visitor latch;
    latch_visitor_init(&latch, stop_on_first);
    my_list_filter filter;
    list_filter_init(&filter, filter_func, filter_cookie, inverted, (list_visitor *) &latch);
    enumeration_result res = list_filter_visit_list(&filter, list);
    assert(ENUMERATION_COMPLETE == res || ENUMERATION_INTERRUPTED == res);
    return latch.last_seen_item;
}

generic_list *list_find_first(generic_list *list, generic_list_filter_func filter_func, void *filter_cookie)
{
    return list_find(list, filter_func, filter_cookie, TOY_TRUE, TOY_FALSE);
}

generic_list *list_find_first_not(generic_list *list, generic_list_filter_func filter_func, void *filter_cookie)
{
    return list_find(list, filter_func, filter_cookie, TOY_TRUE, TOY_TRUE);
}

generic_list *list_find_last(generic_list *list, generic_list_filter_func filter_func, void *filter_cookie)
{
    /* TODO: Should use list_reverse(list) or reverse visitor */
    return list_find(list, filter_func, filter_cookie, TOY_FALSE, TOY_FALSE);
}

generic_list *list_find_last_not(generic_list *list, generic_list_filter_func filter_func, void *filter_cookie)
{
    /* TODO: Should use list_reverse(list) or reverse visitor */
    return list_find(list, filter_func, filter_cookie, TOY_FALSE, TOY_TRUE);
}

struct const_my_list_filter_struct;
typedef struct const_my_list_filter_struct const_my_list_filter;
struct const_my_list_filter_struct {
    const_list_visitor base;
    generic_list_filter_func filter_func;
    void *filter_cookie;
    toy_bool inverted;
    const_list_visitor *next_visitor;
};

item_callback_result const_list_filter_visit_entry(const_my_list_filter *filter, size_t index, const generic_list *entry)
{
    assert((void *) filter == (void *) &filter->base);
    toy_bool filter_res = filter->filter_func(filter->filter_cookie, index, entry);
    if (filter->inverted) {
        filter_res = !filter_res;
    }
    if (filter_res) {
        const_list_visitor *next = filter->next_visitor;
        assert(next);
        return const_list_visitor_visit_entry(next, index, entry);
    }
    return CONTINUE_ENUMERATION;
}

enumeration_result const_list_filter_visit_list(const_my_list_filter *filter, const generic_list *list)
{
    return const_list_visitor_visit_list(&filter->base, list);
}

void const_list_filter_init(const_my_list_filter *filter, generic_list_filter_func filter_func, void *filter_cookie, toy_bool inverted, const_list_visitor *next_visitor)
{
    filter->base.visit_list = NULL;
    filter->base.visit_entry = (const_list_entry_visit_func) const_list_filter_visit_entry;
    filter->filter_func = filter_func;
    filter->filter_cookie = filter_cookie;
    filter->inverted = inverted;
    filter->next_visitor = next_visitor;
}

/* TODO: Cookie support? Since these don't use consumers */
const generic_list *list_find_const(const generic_list *list, generic_list_filter_func filter_func, void *filter_cookie, toy_bool stop_on_first, toy_bool inverted)
{
    latch_visitor latch;
    latch_visitor_init(&latch, stop_on_first);
    const_my_list_filter filter;
    const_list_filter_init(&filter, filter_func, filter_cookie, inverted, (const_list_visitor *) &latch);
    enumeration_result res = const_list_filter_visit_list(&filter, list);
    assert(ENUMERATION_COMPLETE == res || ENUMERATION_INTERRUPTED == res);
    return latch.last_seen_item;
}

const generic_list *list_find_first_const(const generic_list *list, generic_list_filter_func filter_func, void *filter_cookie)
{
    return list_find_const(list, filter_func, filter_cookie, TOY_TRUE, TOY_FALSE);
}

const generic_list *list_find_first_not_const(const generic_list *list, generic_list_filter_func filter_func, void *filter_cookie)
{
    return list_find_const(list, filter_func, filter_cookie, TOY_TRUE, TOY_TRUE);
}

const generic_list *list_find_last_const(const generic_list *list, generic_list_filter_func filter_func, void *filter_cookie)
{
    /* TODO: Should use list_reverse(list) or reverse visitor */
    return list_find_const(list, filter_func, filter_cookie, TOY_FALSE, TOY_FALSE);
}

const generic_list *list_find_last_not_const(const generic_list *list, generic_list_filter_func filter_func, void *filter_cookie)
{
    /* TODO: Should use list_reverse(list) or reverse visitor */
    return list_find_const(list, filter_func, filter_cookie, TOY_FALSE, TOY_TRUE);
}

toy_bool list_all_match(const generic_list *list, generic_list_filter_func filter_func, void *filter_cookie)
{
    const generic_list *match = list_find_first_not_const(list, filter_func, filter_cookie);
    return match == NULL;
}

toy_bool list_none_match(const generic_list *list, generic_list_filter_func filter_func, void *filter_cookie)
{
    const generic_list *match = list_find_first_const(list, filter_func, filter_cookie);
    return match == NULL;
}

toy_bool list_not_all_match(const generic_list *list, generic_list_filter_func filter_func, void *filter_cookie)
{
    return !list_all_match(list, filter_func, filter_cookie);
}

toy_bool list_some_match(const generic_list *list, generic_list_filter_func filter_func, void *filter_cookie)
{
    return !list_none_match(list, filter_func, filter_cookie);
}

static toy_bool compare_to_second_item(void *cookie, size_t index, const toy_str_list *item)
{
    my_list_filter *visitor = (my_list_filter *) cookie;
    assert(visitor);
    assert(visitor->filter_func);
    const toy_str str = str_list_payload_const(item);
    assert(str);
    return str_equal(str, "Second item");
}

static item_callback_result receive_entry(const_str_list_visitor *visitor, size_t index, const toy_str_list *entry)
{
    const toy_str str = str_list_payload_const(entry);
    assert(str_equal(str, "Second item"));
    return CONTINUE_ENUMERATION;
}

static void test_filters(toy_str_list *list)
{
    const_list_visitor receiver = {
        .visit_list = NULL,
        .visit_entry = (const_list_entry_visit_func) receive_entry
    };
    const_my_list_filter filter;
    const_list_filter_init(&filter, (generic_list_filter_func) compare_to_second_item, &filter, TOY_FALSE, &receiver);
    enumeration_result res = const_list_filter_visit_list(&filter, (const generic_list *) list);
    assert(ENUMERATION_COMPLETE == res || ENUMERATION_INTERRUPTED == res);
}

static void test_visitors(toy_str_list *list)
{
    /* TODO */
}

static toy_bool matches_all(void *cookie, size_t index, const toy_str_list *item)
{
    assert(!cookie);
    toy_str str = str_list_payload_const(item);
    return str_contains_nocase(str, "item");
}

static toy_bool matches_none(void *cookie, size_t index, const toy_str_list *item)
{
    assert(!cookie);
    toy_str str = str_list_payload_const(item);
    return str_contains_nocase(str, "this should not be present and contains utter garbage like shinfandoodle");
}

static toy_bool matches_proper_subset(void *cookie, size_t index, const toy_str_list *item)
{
    assert(!cookie);
    toy_str str = str_list_payload_const(item);
    return str_contains_nocase(str, "Third") || str_contains_nocase(str, "Fourth");
}

static void test_set_algebra(toy_str_list *list)
{
    assert(list_all_match((generic_list *) list, (generic_list_filter_func) matches_all, NULL));
    assert(list_none_match((generic_list *) list, (generic_list_filter_func) matches_none, NULL));
    assert(list_not_all_match((generic_list *) list, (generic_list_filter_func) matches_proper_subset, NULL));
    assert(list_some_match((generic_list *) list, (generic_list_filter_func) matches_proper_subset, NULL));
}

void test_lists(void)
{
    toy_str_list *list = str_list_alloc("First item");
    list = str_list_append(list, "Second item");
    list = str_list_append(list, "Third item");
    list = str_list_append(list, "Fourth item");
    list = str_list_append(list, "Fifth item");
    test_visitors(list);
    test_filters(list);
    test_set_algebra(list);
    str_list_free(list);
}
