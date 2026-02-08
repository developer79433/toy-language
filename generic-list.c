#include <stddef.h>
#include <assert.h>
#include <string.h>

#include "bool-types.h"
#include "mymalloc.h"
#include "generic-list.h"
#include "list-visitor.h"
#include "list-filter.h"
#include "list-latch.h"
#include "list-pipeline.h"
#include "log.h"

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

static item_callback_result free_item_cb(list_visitor *list_vis, size_t index, generic_list *list)
{
    free(list);
    return CONTINUE_ENUMERATION;
}

void list_free(generic_list *list)
{
    list_visitor list_vis = { .visit_entry = free_item_cb };
    enumeration_result res = list_visitor_visit_list(&list_vis, list);
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

typedef struct count_visitor_struct {
    const_list_visitor list_vis;
    size_t count;
} count_visitor;

static item_callback_result increment_count_callback(count_visitor *count_vis, size_t index, const generic_list *item)
{
    count_vis->count++;
    return CONTINUE_ENUMERATION;
}

size_t list_len(const generic_list *list)
{
    assert(sizeof(big_list *) == sizeof(small_list *));
    assert(sizeof(generic_list *) == sizeof(small_list *));
    assert(offsetof(big_list, next) == offsetof(small_list, next));
    assert(offsetof(generic_list, next) == offsetof(small_list, next));
    count_visitor count_vis = { .list_vis.visit_entry = (const_list_entry_visit_func) increment_count_callback, .count = 0 };
    enumeration_result res = const_list_visitor_visit_list((const_list_visitor *) &count_vis, list);
    assert(ENUMERATION_COMPLETE == res);
    return count_vis.count;
}

static toy_bool has_null_next(void *cookie, size_t index, const generic_list *item)
{
    return list_next_const(item) == NULL;
}

generic_list *list_last(generic_list *list, generic_list **prev)
{
    assert(list);
    generic_list *last = list_find_first(list, has_null_next, NULL, prev);
    assert(last);
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

typedef struct list_stop_filter_visitor_struct {
    list_visitor list_vis;
    list_filter *filter;
    toy_bool stop_on_match;
} list_stop_filter_visitor;

static item_callback_result stop_visitor_visit_entry(list_stop_filter_visitor *stop_filt_vis, generic_list *item)
{
    list_filter *filter = stop_filt_vis->filter;
    if (list_filter_last_match(filter) && stop_filt_vis->stop_on_match) {
        return STOP_ENUMERATION;
    }
    return CONTINUE_ENUMERATION;
}

static void list_stop_filter_visitor_init(list_stop_filter_visitor *stop_filt_vis, list_filter *filter, toy_bool stop_on_match)
{
    stop_filt_vis->list_vis.visit_entry = (list_entry_visit_func) stop_visitor_visit_entry;
    stop_filt_vis->filter = filter;
    stop_filt_vis->stop_on_match = stop_on_match;
}

/* #define DEBUG_LIST_LATCH_FILTER */

typedef struct list_latch_filter_struct {
    list_latch latch;
    list_filter *filter;
} list_latch_filter;

static item_callback_result list_latch_filter_visit_entry(list_latch_filter *latch_filt, size_t index, generic_list *item)
{
    list_filter *filter = latch_filt->filter;
    if (list_filter_last_match(filter)) {
        list_latch *latch = &latch_filt->latch;
#ifdef DEBUG_LIST_LATCH_FILTER
    log_printf_file("filter succeeded, so letting latch visit entry %p\n", item);
#endif /* DEBUG_LIST_LATCH_FILTER */
        return list_latch_visit_entry(latch, index, item);
    }
#ifdef DEBUG_LIST_LATCH_FILTER
    log_printf_file("filter failed, so continuing\n");
#endif /* DEBUG_LIST_LATCH_FILTER */
    return CONTINUE_ENUMERATION;
}

void list_latch_filter_init(list_latch_filter *latch_filt, list_filter *filter)
{
    list_latch *latch = &latch_filt->latch;
    latch_filt->filter = filter;
    list_latch_init(latch);
    latch->visitor.visit_entry = (list_entry_visit_func) list_latch_filter_visit_entry;
}

generic_list *list_latch_filter_get_last_seen(list_latch_filter *latch_filt)
{
    list_filter *filter = latch_filt->filter;
    return list_filter_last_match(filter);
}

static toy_bool item_equals(const generic_list *compare_to, size_t index, const generic_list *entry)
{
    return compare_to == entry;
}

toy_bool list_contains(const generic_list *list, const generic_list *entry)
{
    return list_some_match(list, (generic_list_filter_func) item_equals, (void *) entry);
}

void list_assert_contains(const generic_list *list, const generic_list *entry)
{
    assert(list_contains(list, entry));
}

static generic_list *list_find(generic_list *list, generic_list_filter_func filter_func, void *filter_cookie, toy_bool stop_on_first, toy_bool inverted, generic_list **pprev)
{
    list_filter filter;
    list_filter_init(&filter, filter_func, filter_cookie, inverted);
    assert(NULL == list_visitor_prev_item((list_visitor *) &filter));
    list_stop_filter_visitor stop_filt_vis;
    list_stop_filter_visitor_init(&stop_filt_vis, &filter, stop_on_first);
    list_latch_filter save_match_vis;
    list_latch_filter_init(&save_match_vis, &filter);
    list_pipeline stop_step = { .visitor = (list_visitor *) &stop_filt_vis, .next = NULL };
    list_pipeline save_match_step = { .visitor = (list_visitor *) &save_match_vis, .next = &stop_step };
    list_pipeline filter_step = { .visitor = (list_visitor *) &filter, .next = &save_match_step };
    list_pipeline *pipeline = &filter_step;
    enumeration_result res = list_pipeline_visit_list(pipeline, list);
    assert(ENUMERATION_COMPLETE == res || ENUMERATION_INTERRUPTED == res);
    generic_list *prev_item = list_visitor_prev_item((list_visitor *) &filter);
    list_assert_contains(list, list);
    if (prev_item) {
        list_assert_contains(list, prev_item);
    }
    generic_list *match = list_latch_filter_get_last_seen(&save_match_vis);
    assert(
        !prev_item
        || !match
        || prev_item != match
    );
    if (pprev) {
        *pprev = prev_item;
    }
    return match;
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

typedef struct const_list_stop_filter_visitor_struct {
    const_list_visitor list_vis;
    const_list_filter *filter;
    toy_bool stop_on_first;
} const_list_stop_filter_visitor;

static item_callback_result const_stop_visitor_visit_entry(const_list_stop_filter_visitor *stop_filt_vis, const generic_list *item)
{
    if (const_list_filter_last_match(stop_filt_vis->filter) && stop_filt_vis->stop_on_first) {
        return STOP_ENUMERATION;
    }
    return CONTINUE_ENUMERATION;
}

static void const_list_stop_filter_visitor_init(const_list_stop_filter_visitor *stop_filt_vis, const_list_filter *filter, toy_bool stop_on_first)
{
    stop_filt_vis->list_vis.visit_entry = (const_list_entry_visit_func) const_stop_visitor_visit_entry;
    stop_filt_vis->filter = filter;
    stop_filt_vis->stop_on_first = stop_on_first;
}

typedef struct const_list_latch_filter_struct {
    const_list_latch latch;
    const_list_filter *filter;
} const_list_latch_filter;

static item_callback_result const_list_latch_filter_visit_entry(const_list_latch_filter *latch_filt, size_t index, const generic_list *item)
{
    const_list_filter *filter = latch_filt->filter;
    if (const_list_filter_last_match(filter)) {
        const_list_latch *latch = &latch_filt->latch;
#ifdef DEBUG_LIST_LATCH_FILTER
    log_printf_file("filter succeeded, so letting latch visit entry %p\n", item);
#endif /* DEBUG_LIST_LATCH_FILTER */
        return const_list_latch_visit_entry(latch, index, item);
    }
#ifdef DEBUG_LIST_LATCH_FILTER
    log_printf_file("filter failed, so continuing\n");
#endif /* DEBUG_LIST_LATCH_FILTER */
    return CONTINUE_ENUMERATION;
}

void const_list_latch_filter_init(const_list_latch_filter *latch_filt, const_list_filter *filter)
{
    const_list_latch *latch = &latch_filt->latch;
    latch_filt->filter = filter;
    const_list_latch_init(latch);
    latch->visitor.visit_entry = (const_list_entry_visit_func) const_list_latch_filter_visit_entry;
}

const generic_list *const_list_latch_filter_get_last_seen(const_list_latch_filter *latch_filt)
{
    const_list_filter *filter = latch_filt->filter;
    return const_list_filter_last_match(filter);
}

static const generic_list *list_find_const(const generic_list *list, generic_list_filter_func filter_func, void *filter_cookie, toy_bool stop_on_first, toy_bool inverted)
{
    const_list_filter filter;
    const_list_filter_init(&filter, filter_func, filter_cookie, inverted);
    const_list_stop_filter_visitor stop_filt_vis;
    const_list_stop_filter_visitor_init(&stop_filt_vis, &filter, stop_on_first);
    const_list_latch_filter save_match_vis;
    const_list_latch_filter_init(&save_match_vis, &filter);
    const_list_pipeline stop_step = { .visitor = (const_list_visitor *) &stop_filt_vis, .next = NULL };
    const_list_pipeline save_match_step = { .visitor = (const_list_visitor *) &save_match_vis, .next = &stop_step };
    const_list_pipeline filter_step = { .visitor = (const_list_visitor *) &filter, .next = &save_match_step };
    const_list_pipeline *pipeline = &filter_step;
    enumeration_result res = const_list_pipeline_visit_list(pipeline, list);
    assert(ENUMERATION_COMPLETE == res || ENUMERATION_INTERRUPTED == res);
    const generic_list *match = const_list_latch_filter_get_last_seen(&save_match_vis);
    return match;
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

static generic_list *list_remove_entry(generic_list *list, generic_list *to_remove, generic_list *prev)
{
    assert(list);
    assert(to_remove);
    assert(prev != to_remove);
    generic_list *next = to_remove->next;
    to_remove->next = NULL;
    if (prev) {
        prev->next = next;
    }
    if (to_remove == list) {
        list = next;
    }
    return list;
}

typedef generic_list *(*list_find_func)(generic_list *list, generic_list_filter_func filter_func, void *filter_cookie, generic_list **prev);

static generic_list *list_delete_find_func(generic_list *list, list_find_func find_func, generic_list_filter_func filter_func, void *filter_cookie, list_entry_free_func free_func, delete_result *del_res)
{
    generic_list *prev = NULL;
    generic_list *found = find_func(
        list,
        filter_func,
        filter_cookie,
        &prev
    );
    if (found) {
        assert(prev != found);
        *del_res = DELETED;
        list = list_remove_entry(list, found, prev);
        free_func(found);
    } else {
        *del_res = NOT_PRESENT;
    }
    return list;
}

generic_list *list_delete_first(generic_list *list, generic_list_filter_func filter_func, void *filter_cookie, list_entry_free_func free_func, delete_result *del_res)
{
    return list_delete_find_func(list, list_find_first, filter_func, filter_cookie, free_func, del_res);
}

generic_list *list_delete_first_not(generic_list *list, generic_list_filter_func filter_func, void *filter_cookie, list_entry_free_func free_func, delete_result *del_res)
{
    return list_delete_find_func(list, list_find_first_not, filter_func, filter_cookie, free_func, del_res);
}

generic_list *list_delete_last(generic_list *list, generic_list_filter_func filter_func, void *filter_cookie, list_entry_free_func free_func, delete_result *del_res)
{
    return list_delete_find_func(list, list_find_last, filter_func, filter_cookie, free_func, del_res);
}

generic_list *list_delete_last_not(generic_list *list, generic_list_filter_func filter_func, void *filter_cookie, list_entry_free_func free_func, delete_result *del_res)
{
    return list_delete_find_func(list, list_find_last_not, filter_func, filter_cookie, free_func, del_res);
}

typedef struct delete_visitor_struct {
    list_visitor list_vis;
    list_filter *filter;
    generic_list *list;
    list_entry_free_func free_func;
} delete_visitor;

static item_callback_result delete_visitor_visit_entry(delete_visitor *del_vis, size_t index, generic_list *item)
{
    list_filter *filter = del_vis->filter;
    generic_list *last_match = list_filter_last_match(filter);
    if (last_match) {
        assert(last_match == item);
        generic_list *prev = list_visitor_prev_item((list_visitor *) filter);
        del_vis->list = list_remove_entry(del_vis->list, item, prev);
        del_vis->free_func(item);
    }
    return CONTINUE_ENUMERATION;
}

static void delete_visitor_init(delete_visitor *del_vis, generic_list *list, list_filter *filter, list_entry_free_func free_func)
{
    del_vis->list_vis.visit_entry = (list_entry_visit_func) delete_visitor_visit_entry;
    del_vis->list = list;
    del_vis->free_func = free_func;
    del_vis->filter = filter;
}

generic_list *list_delete_filter(generic_list *list, generic_list_filter_func filter_func, void *filter_cookie, toy_bool stop_on_first, toy_bool inverted, list_entry_free_func free_func)
{
    list_filter filter;
    list_filter_init(&filter, filter_func, filter_cookie, inverted);
    list_stop_filter_visitor stop_filt_vis;
    list_stop_filter_visitor_init(&stop_filt_vis, &filter, stop_on_first);
    delete_visitor del_vis;
    delete_visitor_init(&del_vis, list, &filter, free_func);
    list_pipeline optional_stop_step = { .visitor = &stop_visitor, .next = NULL };
    list_pipeline delete_step = { .visitor = (list_visitor *) &del_vis, .next = &optional_stop_step };
    list_pipeline filter_step = { .visitor = (list_visitor *) &filter, .next = &delete_step };
    list_pipeline *delete_pipeline = &filter_step;

    enumeration_result res = list_pipeline_visit_list(delete_pipeline, list);
    assert(ENUMERATION_COMPLETE == res || ENUMERATION_INTERRUPTED == res);
    return del_vis.list;
}

void list_assert_all_match(const generic_list *list, generic_list_filter_func filter_func, void *filter_cookie)
{
    const generic_list *first_mismatch = list_find_first_not_const(list, filter_func, filter_cookie);
    assert(first_mismatch == NULL);
}

void list_assert_none_match(const generic_list *list, generic_list_filter_func filter_func, void *filter_cookie)
{
    const generic_list *first_match = list_find_first_const(list, filter_func, filter_cookie);
    assert(first_match == NULL);
}

void list_assert_not_all_match(const generic_list *list, generic_list_filter_func filter_func, void *filter_cookie)
{
    const generic_list *first_mismatch = list_find_first_not_const(list, filter_func, filter_cookie);
    assert(first_mismatch != NULL);
}

void list_assert_some_match(const generic_list *list, generic_list_filter_func filter_func, void *filter_cookie)
{
    const generic_list *first_match = list_find_first_const(list, filter_func, filter_cookie);
    assert(first_match != NULL);
}
