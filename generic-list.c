#include <stddef.h>
#include <assert.h>

#include "generic-list.h"

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

/* TODO: Handle orig being null, so the caller in parser.y doesn't have to repeatedly do so */
generic_list *generic_list_append(generic_list *orig_list, generic_list *new_list)
{
    /* FIXME: inefficient */
    generic_list *tmp = orig_list;
    while (tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = new_list;
    return orig_list;
}

void *generic_list_index(generic_list *list, toy_num index)
{
    if (index < 0) {
        return NULL;
    }
    /* FIXME: inefficient */
    for (size_t i = 0; list; list = list->next, i++) {
        if (i == index) {
            return list->payload;
        }
    }
    return NULL;
}

void generic_list_foreach(void *listv, void *cookie, list_item_callback callback)
{
    generic_list *list = (generic_list *) listv;
    for (; list; list = list->next) {
        callback(cookie, list->payload);
    }
}

void generic_list_foreach_const(const void *listv, void *cookie, const_list_item_callback callback)
{
    const generic_list *list = (const generic_list *) listv;
    for (; list; list = list->next) {
        callback(cookie, list->payload);
    }
}
