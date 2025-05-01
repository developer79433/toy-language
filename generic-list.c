#include <stddef.h>
#include <assert.h>
#include <string.h>

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

size_t generic_list_len(const generic_list *list)
{
    assert(sizeof(big_list *) == sizeof(small_list *));
    assert(sizeof(generic_list *) == sizeof(small_list *));
    assert(offsetof(big_list, next) == offsetof(small_list, next));
    assert(offsetof(generic_list, next) == offsetof(small_list, next));
    size_t size;
    for (size = 0; list; list = list->next) {
        size++;
    }
    return size;
}

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

/* TODO: Handle orig being null, so the caller in parser.y doesn't have to repeatedly do so */
generic_list *generic_list_concat(generic_list *orig_list, generic_list *new_list)
{
    generic_list *tmp = orig_list;
    while (tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = new_list;
    return orig_list;
}

list_iter_result generic_list_foreach(generic_list *list, generic_list_item_callback callback, void *cookie)
{
    for (size_t i = 0; list; i++) {
        generic_list *next = list->next;
        int ret = callback(cookie, i, list);
        if (STOP_ITERATING == ret) {
            return CALLBACK_STOPPED;
        }
        list = next;
    }
    return REACHED_END;
}

list_iter_result generic_list_foreach_const(const generic_list *list, const_generic_list_item_callback callback, void *cookie)
{
    for (size_t i = 0; list; i++) {
        const generic_list *next = list->next;
        int ret = callback(cookie, i, list);
        if (STOP_ITERATING == ret) {
            return CALLBACK_STOPPED;
        }
        list = next;
    }
    return REACHED_END;
}

void generic_list_free(generic_list *list)
{
    while (list) {
        generic_list *next = list->next;
        free(list);
        list = next;
    }
}

typedef struct compare_indices_args_struct {
    generic_list_item_callback user_callback;
    void *user_cookie;
    size_t desired_index;
} compare_indices_args;

static listitem_callback_result compare_indices(void *cookie, size_t index, generic_list *item)
{
    compare_indices_args *args = (compare_indices_args *) cookie;
    if (index == args->desired_index) {
        args->user_callback(args->user_cookie, index, item);
        return STOP_ITERATING;
    }
    return KEEP_ITERATING;
}

list_iter_result generic_list_index(generic_list *list, size_t index, generic_list_item_callback callback, void *cookie)
{
    compare_indices_args args = { .user_callback = callback, .user_cookie = cookie, .desired_index = index };
    list_iter_result res = generic_list_foreach(list, compare_indices, &args);
    return res;
}
