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

generic_list *generic_list_append(generic_list *orig_list, generic_list *new_list)
{
    generic_list *tmp = orig_list;
    while (tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = new_list;
    return orig_list;
}
