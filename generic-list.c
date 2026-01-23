#include <stddef.h>
#include <assert.h>
#include <string.h>

#include "bool-types.h"
#include "mymalloc.h"
#include "generic-list.h"
#include "list-visitor.h"
#include "list-filter.h"
#include "list-latch.h"
#include "list-latch-prev.h"

void *INDEX_OUT_OF_BOUNDS = (void *) 1;

typedef struct big_list_struct {
    struct big_list_struct *next;
    char c;
    int i;
    double d;
    float f;
    int *intptr;
    int (*funcptr)(double *d, int i);
} big_list;

typedef struct small_list_struct {
    struct small_list_struct *next;
    char c;
} small_list;

generic_list *list_alloc_size(size_t payload_size)
{
    generic_list *list;
    list = (generic_list *) malloc(
        /* size of the list structure itself */
        sizeof(generic_list)
        /* add size of actual payload */
        + payload_size
    );
    list->next = NULL;
    return list;
}

generic_list *list_next(generic_list *list)
{
    return list->next;
}

const generic_list *list_next_const(const generic_list *list)
{
    return list->next;
}

enumeration_result list_foreach(generic_list *list, generic_list_item_callback callback, void *cookie)
{
    for (size_t i = 0; list; i++) {
        generic_list *next = list_next(list);
        item_callback_result res = callback(cookie, i, list);
        if (STOP_ENUMERATION == res) {
            return ENUMERATION_INTERRUPTED;
        }
        list = next;
    }
    return ENUMERATION_COMPLETE;
}

enumeration_result list_foreach_const(const generic_list *list, const_generic_list_item_callback callback, void *cookie)
{
    for (size_t i = 0; list; i++) {
        const generic_list *next = list_next_const(list);
        item_callback_result res = callback(cookie, i, list);
        if (STOP_ENUMERATION == res) {
            return ENUMERATION_INTERRUPTED;
        }
        list = next;
    }
    return ENUMERATION_COMPLETE;
}

static item_callback_result free_item_cb(void *cookie, size_t index, generic_list *list)
{
    free(list);
    return CONTINUE_ENUMERATION;
}

void list_free(generic_list *list)
{
    enumeration_result res = list_foreach(list, free_item_cb, NULL);
    assert(res == ENUMERATION_COMPLETE);
}

static toy_bool is_desired_index(void *cookie, size_t index, const generic_list *item)
{
    size_t *desired_index = (size_t *) cookie;
    return (index == *desired_index);
}

generic_list *list_index(generic_list *list, size_t index)
{
    return list_find_first(list, is_desired_index, &index, NULL);
}

const generic_list *list_index_const(const generic_list *list, size_t index)
{
    return list_find_first_const(list, is_desired_index, &index);
}

static item_callback_result increment_count_callback(void *cookie, size_t index, const generic_list *item)
{
    size_t *counter = (size_t *) cookie;
    (*counter)++;
    return CONTINUE_ENUMERATION;
}

size_t list_len(const generic_list *list)
{
    assert(sizeof(big_list *) == sizeof(small_list *));
    assert(sizeof(generic_list *) == sizeof(small_list *));
    assert(offsetof(big_list, next) == offsetof(small_list, next));
    assert(offsetof(generic_list, next) == offsetof(small_list, next));
    size_t size = 0;
    enumeration_result res = list_foreach_const(list, increment_count_callback, &size);
    assert(ENUMERATION_COMPLETE == res);
    return size;
}

static toy_bool has_null_next(void *cookie, size_t index, const generic_list *item)
{
    return list_next_const(item) == NULL;
}

generic_list *list_last(generic_list *list, generic_list **prev)
{
    generic_list *last = list_find_first(list, has_null_next, NULL, prev);
    assert(NULL == list_next_const(last));
    return last;
}

generic_list *list_concat(generic_list *list, generic_list *new_list)
{
    generic_list *last = list_last(list, NULL);
    assert(NULL == list_next_const(last));
    last->next = new_list;
    return list;
}

generic_list *list_remove_first(generic_list *list, generic_list **removed)
{
    assert(list);
    if (removed) {
        *removed = list;
    }
    generic_list *ret = list_next(list);
    list->next = NULL;
    return ret;
}

generic_list *list_remove_last(generic_list *list, generic_list **removed)
{
    assert(list);
    generic_list *prev = NULL;
    generic_list *last = list_last(list, &prev);
    assert(NULL == list_next_const(last));
    if (prev == NULL) {
        assert(last == list);
        assert(NULL == list_next_const(list));
        list = NULL;
    } else {
        assert(last != list);
        assert(list_next_const(prev) != NULL);
        assert(list_next_const(prev) == last);
        prev->next = NULL;
    }
    if (removed) {
        *removed = last;
    }
    return list;
}

static generic_list *list_find(generic_list *list, generic_list_filter_func filter_func, void *filter_cookie, toy_bool stop_on_first, toy_bool inverted, generic_list **prev)
{
    list_latch_prev latch_prev;
    list_latch_prev_init(&latch_prev, list, stop_on_first);
    my_list_filter filter;
    list_filter_init(&filter, filter_func, filter_cookie, inverted, (list_visitor *) &latch_prev);
    enumeration_result res = list_filter_visit_list(&filter, list);
    assert(ENUMERATION_COMPLETE == res || ENUMERATION_INTERRUPTED == res);
    if (prev) {
        *prev = latch_prev.prev;
    }
    return latch_prev.latch.last_seen_item;
}

generic_list *list_find_first(generic_list *list, generic_list_filter_func filter_func, void *filter_cookie, generic_list **prev)
{
    return list_find(list, filter_func, filter_cookie, TOY_TRUE, TOY_FALSE, prev);
}

generic_list *list_find_first_not(generic_list *list, generic_list_filter_func filter_func, void *filter_cookie, generic_list **prev)
{
    return list_find(list, filter_func, filter_cookie, TOY_TRUE, TOY_TRUE, prev);
}

generic_list *list_find_last(generic_list *list, generic_list_filter_func filter_func, void *filter_cookie, generic_list **prev)
{
    /* TODO: Should use list_reverse(list) or reverse visitor */
    return list_find(list, filter_func, filter_cookie, TOY_FALSE, TOY_FALSE, prev);
}

generic_list *list_find_last_not(generic_list *list, generic_list_filter_func filter_func, void *filter_cookie, generic_list **prev)
{
    /* TODO: Should use list_reverse(list) or reverse visitor */
    return list_find(list, filter_func, filter_cookie, TOY_FALSE, TOY_TRUE, prev);
}

static const generic_list *list_find_const(const generic_list *list, generic_list_filter_func filter_func, void *filter_cookie, toy_bool stop_on_first, toy_bool inverted)
{
    list_latch latch;
    list_latch_init(&latch, stop_on_first);
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

toy_bool match_always(void *cookie, size_t index, const generic_list *item)
{
    return TOY_TRUE;
}

toy_bool match_never(void *cookie, size_t index, const generic_list *item)
{
    return TOY_FALSE;
}
