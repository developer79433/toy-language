#include <stddef.h>
#include <assert.h>
#include <string.h>

#include "bool-types.h"
#include "mymalloc.h"
#include "generic-list.h"

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

generic_list *generic_list_alloc_size(size_t payload_size)
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

enumeration_result generic_list_foreach(generic_list *list, generic_list_item_callback callback, void *cookie)
{
    for (size_t i = 0; list; i++) {
        generic_list *next = list->next;
        item_callback_result res = callback(cookie, i, list);
        if (STOP_ENUMERATION == res) {
            return ENUMERATION_INTERRUPTED;
        }
        list = next;
    }
    return ENUMERATION_COMPLETE;
}

enumeration_result generic_list_foreach_const(const generic_list *list, const_generic_list_item_callback callback, void *cookie)
{
    for (size_t i = 0; list; i++) {
        const generic_list *next = list->next;
        item_callback_result res = callback(cookie, i, list);
        if (STOP_ENUMERATION == res) {
            return ENUMERATION_INTERRUPTED;
        }
        list = next;
    }
    return ENUMERATION_COMPLETE;
}

item_callback_result list_visitor_visit_entry_default(list_visitor *visitor, size_t index, generic_list *item)
{
    /* NOP */
    return CONTINUE_ENUMERATION;
}

item_callback_result list_visitor_visit_entry(list_visitor *visitor, size_t index, generic_list *item)
{
    if (visitor->visit_entry) {
        return visitor->visit_entry(visitor, index, item);
    }
    return list_visitor_visit_entry_default(visitor, index, item);
}

enumeration_result list_visitor_visit_list_default(list_visitor *visitor, generic_list *list)
{
    for (size_t i = 0; list; i++) {
        generic_list *next = list->next;
        item_callback_result res = list_visitor_visit_entry(visitor, i, list);
        if (STOP_ENUMERATION == res) {
            return ENUMERATION_INTERRUPTED;
        }
        list = next;
    }
    return ENUMERATION_COMPLETE;
}

enumeration_result list_visitor_visit_list(list_visitor *visitor, generic_list *list)
{
    if (visitor->visit_list) {
        return visitor->visit_list(visitor, list);
    }
    return list_visitor_visit_list_default(visitor, list);
}

item_callback_result const_list_visitor_visit_entry_default(const_list_visitor *visitor, size_t index, const generic_list *item)
{
    /* NOP */
    return CONTINUE_ENUMERATION;
}

item_callback_result const_list_visitor_visit_entry(const_list_visitor *visitor, size_t index, const generic_list *item)
{
    if (visitor->visit_entry) {
        return visitor->visit_entry(visitor, index, item);
    }
    return const_list_visitor_visit_entry_default(visitor, index, item);
}

enumeration_result const_list_visitor_visit_list_default(const_list_visitor *visitor, const generic_list *list)
{
    for (size_t i = 0; list; i++) {
        const generic_list *next = list->next;
        item_callback_result res = const_list_visitor_visit_entry(visitor, i, list);
        if (STOP_ENUMERATION == res) {
            return ENUMERATION_INTERRUPTED;
        }
        list = next;
    }
    return ENUMERATION_COMPLETE;
}

enumeration_result const_list_visitor_visit_list(const_list_visitor *visitor, const generic_list *list)
{
    if (visitor->visit_list) {
        return visitor->visit_list(visitor, list);
    }
    return const_list_visitor_visit_list_default(visitor, list);
}

static item_callback_result free_item_cb(void *cookie, size_t index, generic_list *list)
{
    free(list);
    return CONTINUE_ENUMERATION;
}

void generic_list_free(generic_list *list)
{
    enumeration_result res = generic_list_foreach(list, free_item_cb, NULL);
    assert(res == ENUMERATION_COMPLETE);
}

static item_callback_result find_all_callback(void *cookie, size_t index, generic_list *item)
{
    filter_args *args = (filter_args *) cookie;
    if (args->filter(cookie, index, item)) {
        return args->user_callback(args->user_cookie, index, item);
    }
    return CONTINUE_ENUMERATION;
}

enumeration_result generic_list_filter(generic_list *list, generic_list_filter_func filter, void *filter_cookie, generic_list_item_callback callback, void *cookie)
{
    filter_args args = { .filter = filter, .filter_cookie = filter_cookie, .user_callback = callback, .user_cookie = cookie };
    enumeration_result res = generic_list_foreach(list, find_all_callback, &args);
    return res;
}

static item_callback_result const_find_all_callback(void *cookie, size_t index, const generic_list *item)
{
    const_filter_args *args = (const_filter_args *) cookie;
    if (args->filter(cookie, index, item)) {
        return args->user_callback(args->user_cookie, index, item);
    }
    return CONTINUE_ENUMERATION;
}

enumeration_result generic_list_filter_const(const generic_list *list, generic_list_filter_func filter, void *filter_cookie, const_generic_list_item_callback callback, void *cookie)
{
    const_filter_args args = { .filter = filter, .filter_cookie = filter_cookie, .user_callback = callback, .user_cookie = cookie };
    enumeration_result res = generic_list_foreach_const(list, const_find_all_callback, &args);
    return res;
}

typedef struct find_one_args_struct {
    filter_args filtargs;
    generic_list *prev_item;
    generic_list *found_item;
} find_one_args;

static item_callback_result find_first_callback(void *cookie, size_t index, generic_list *item)
{
    find_one_args *args = (find_one_args *) cookie;
    if (args->filtargs.filter(args->filtargs.user_cookie, index, item)) {
        /* Ignore user callback return value */
        /* FIXME: Shouldn't this set prev_item too? */
        args->found_item = item;
        return STOP_ENUMERATION;
    }
    args->prev_item = item;
    return CONTINUE_ENUMERATION;
}

generic_list *generic_list_find_first(generic_list *list, generic_list_filter_func filter, void *cookie, generic_list **prev)
{
    find_one_args args = { .prev_item = NULL, .found_item = NULL, .filtargs.filter = filter, .filtargs.user_callback = NULL, .filtargs.user_cookie = cookie };
    enumeration_result res = generic_list_foreach(list, find_first_callback, &args);
    assert(
        (
            (res == ENUMERATION_INTERRUPTED) && (args.found_item != NULL)
        ) || (
            (res == ENUMERATION_COMPLETE) && (args.found_item == NULL)
        )
    );
    if (prev) {
        *prev = args.prev_item;
    }
    return args.found_item;
}

typedef struct const_find_one_args_struct {
    filter_args filtargs;
    const generic_list *prev_item;
    const generic_list *found_item;
} const_find_one_args;

static item_callback_result const_find_first_callback(void *cookie, size_t index, const generic_list *item)
{
    const_find_one_args *args = (const_find_one_args *) cookie;
    if (args->filtargs.filter(args->filtargs.user_cookie, index, item)) {
        /* Ignore user callback return value */
        args->found_item = item;
        return STOP_ENUMERATION;
    }
    args->prev_item = item;
    return CONTINUE_ENUMERATION;
}

const generic_list *generic_list_find_first_const(const generic_list *list, generic_list_filter_func filter, void *cookie, const generic_list **prev)
{
    const_find_one_args args = { .prev_item = NULL, .found_item = NULL, .filtargs.filter = filter, .filtargs.user_callback = NULL, .filtargs.user_cookie = cookie };
    enumeration_result res = generic_list_foreach_const(list, const_find_first_callback, &args);
    assert(
        (
            (res == ENUMERATION_INTERRUPTED) && (args.found_item != NULL)
        ) || (
            (res == ENUMERATION_COMPLETE) && (args.found_item == NULL)
        )
    );
    if (prev) {
        *prev = args.prev_item;
    }
    return args.found_item;
}

toy_bool generic_list_none_match(const generic_list *list, generic_list_filter_func filter, void *cookie)
{
    const generic_list *found = generic_list_find_first_const(list, filter, cookie, NULL);
    return (found == NULL);
}

static item_callback_result find_first_not_callback(void *cookie, size_t index, generic_list *item)
{
    find_one_args *args = (find_one_args *) cookie;
    if (!args->filtargs.filter(args->filtargs.user_cookie, index, item)) {
        /* Ignore user callback return value */
        args->found_item = item;
        return STOP_ENUMERATION;
    }
    return CONTINUE_ENUMERATION;
}

generic_list *generic_list_find_first_not(generic_list *list, generic_list_filter_func filter, void *cookie)
{
    find_one_args args = { .prev_item = NULL, .found_item = NULL, .filtargs.filter = filter, .filtargs.user_callback = NULL, .filtargs.user_cookie = cookie };
    enumeration_result res = generic_list_foreach(list, find_first_not_callback, &args);
    assert(
        (
            (res == ENUMERATION_INTERRUPTED) && (args.found_item != NULL)
        ) || (
            (res == ENUMERATION_COMPLETE) && (args.found_item == NULL)
        )
    );
    return args.found_item;
}

static item_callback_result find_first_not_const_callback(void *cookie, size_t index, const generic_list *item)
{
    const_find_one_args *args = (const_find_one_args *) cookie;
    if (!args->filtargs.filter(args->filtargs.user_cookie, index, item)) {
        /* Ignore user callback return value */
        args->found_item = item;
        return STOP_ENUMERATION;
    }
    return CONTINUE_ENUMERATION;
}

const generic_list *generic_list_find_first_not_const(const generic_list *list, generic_list_filter_func filter, void *cookie)
{
    find_one_args args = { .prev_item = NULL, .found_item = NULL, .filtargs.filter = filter, .filtargs.user_callback = NULL, .filtargs.user_cookie = cookie };
    enumeration_result res = generic_list_foreach_const(list, find_first_not_const_callback, &args);
    assert(
        (
            (res == ENUMERATION_INTERRUPTED) && (args.found_item != NULL)
        ) || (
            (res == ENUMERATION_COMPLETE) && (args.found_item == NULL)
        )
    );
    return args.found_item;
}

toy_bool generic_list_all_match(const generic_list *list, generic_list_filter_func filter, void *cookie)
{
    const generic_list *found = generic_list_find_first_not_const(list, filter, cookie);
    return (found == NULL);
}

static toy_bool is_desired_index(void *cookie, size_t index, const generic_list *item)
{
    size_t *desired_index = (size_t *) cookie;
    return (index == *desired_index);
}

generic_list *generic_list_index(generic_list *list, size_t index)
{
    return generic_list_find_first(list, is_desired_index, &index, NULL);
}

const generic_list *generic_list_index_const(const generic_list *list, size_t index)
{
    return generic_list_find_first_const(list, is_desired_index, &index, NULL);
}

static item_callback_result increment_count_callback(void *cookie, size_t index, const generic_list *item)
{
    size_t *counter = (size_t *) cookie;
    (*counter)++;
    return CONTINUE_ENUMERATION;
}

size_t generic_list_len(const generic_list *list)
{
    assert(sizeof(big_list *) == sizeof(small_list *));
    assert(sizeof(generic_list *) == sizeof(small_list *));
    assert(offsetof(big_list, next) == offsetof(small_list, next));
    assert(offsetof(generic_list, next) == offsetof(small_list, next));
    size_t size = 0;
    enumeration_result res = generic_list_foreach_const(list, increment_count_callback, &size);
    assert(ENUMERATION_COMPLETE == res);
    return size;
}

static toy_bool has_null_next(void *cookie, size_t index, const generic_list *item)
{
    return item->next == NULL;
}

generic_list *generic_list_last(generic_list *list, generic_list **prev)
{
    generic_list *last = generic_list_find_first(list, has_null_next, NULL, prev);
    assert(NULL == last->next);
    return last;
}

generic_list *generic_list_concat(generic_list *list, generic_list *new_list)
{
    generic_list *last = generic_list_last(list, NULL);
    assert(NULL == last->next);
    last->next = new_list;
    return list;
}

generic_list *generic_list_remove_first(generic_list *list, generic_list **removed)
{
    assert(list);
    if (removed) {
        *removed = list;
    }
    generic_list *ret = list->next;
    list->next = NULL;
    return ret;
}

generic_list *generic_list_remove_last(generic_list *list, generic_list **removed)
{
    assert(list);
    generic_list *prev = NULL;
    generic_list *last = generic_list_last(list, &prev);
    assert(NULL == last->next);
    if (prev == NULL) {
        assert(last == list);
        assert(NULL == list->next);
        list = NULL;
    } else {
        assert(last != list);
        assert(prev->next != NULL);
        assert(prev->next == last);
        prev->next = NULL;
    }
    if (removed) {
        *removed = last;
    }
    return list;
}
