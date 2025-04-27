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

generic_list *generic_list_alloc_ref(void *first_elem)
{
    generic_list *list;
    list = mymalloc(generic_list);
    list->payload = first_elem;
    list->next = NULL;
    return list;
}

generic_list *generic_list_alloc_own(void *first_elem, size_t value_size)
{
    generic_list *list;
    list = (generic_list *) malloc(sizeof(generic_list) + value_size);
    list->payload = (void *) (list + 1);
    memcpy(list->payload, first_elem, value_size);
    list->next = NULL;
    return list;
}

/* TODO: Handle orig being null, so the caller in parser.y doesn't have to repeatedly do so */
generic_list *generic_list_concat(generic_list *orig_list, generic_list *new_list)
{
    /* FIXME: inefficient */
    generic_list *tmp = orig_list;
    while (tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = new_list;
    return orig_list;
}

generic_list *generic_list_append_ref(generic_list *list, void *new_payload)
{
    generic_list *new_list;
    new_list = mymalloc(generic_list);
    new_list->payload = new_payload;
    new_list->next = NULL;
    return generic_list_concat(list, new_list);
}

generic_list *generic_list_append_own(generic_list *list, void *new_payload, size_t payload_size)
{
    generic_list *new_list;
    new_list = (generic_list *) malloc(sizeof(generic_list) + payload_size);
    new_list->payload = (void *) (new_list + 1);
    memcpy(new_list->payload, new_payload, payload_size);
    new_list->next = NULL;
    return generic_list_concat(list, new_list);
}

generic_list *generic_list_prepend_ref(generic_list *list, void *new_payload)
{
    generic_list *new_list;
    new_list = mymalloc(generic_list);
    new_list->payload = new_payload;
    new_list->next = list;
    return new_list;
}

generic_list *generic_list_prepend_own(generic_list *list, void *new_payload, size_t payload_size)
{
    generic_list *new_list;
    new_list = (generic_list *) malloc(sizeof(generic_list) + payload_size);
    new_list->payload = (void *) (new_list + 1);
    memcpy(new_list->payload, new_payload, payload_size);
    new_list->next = list;
    return new_list;
}

void *generic_list_index(generic_list *list, size_t index)
{
    /* FIXME: inefficient */
    for (size_t i = 0; list; list = list->next, i++) {
        if (i == index) {
            return list->payload;
        }
    }
    return INDEX_OUT_OF_BOUNDS;
}

void generic_list_foreach(void *listv, list_item_callback callback, void *cookie)
{
    generic_list *list = (generic_list *) listv;
    for (; list; list = list->next) {
        callback(cookie, list->payload);
    }
}

void generic_list_foreach_const(const void *listv, const_list_item_callback callback, void *cookie)
{
    const generic_list *list = (const generic_list *) listv;
    for (; list; list = list->next) {
        callback(cookie, list->payload);
    }
}

void generic_list_free(generic_list *list)
{
    for (generic_list *cur = list; cur; ) {
        generic_list *next = cur->next;
        free(cur);
        cur = next;
    }
}
