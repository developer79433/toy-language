#ifndef GENERIC_LIST_TYPES_H
#define GENERIC_LIST_TYPES_H 1

#include <stddef.h>
#include <inttypes.h>

#include "bool-types.h"
#include "iter-types.h"

typedef struct generic_list_struct {
    struct generic_list_struct *next;
    /* user code should declare additional members in 'subclasses' here */
} generic_list;

extern void *INDEX_OUT_OF_BOUNDS;

typedef toy_bool (*generic_list_filter_func)(void *cookie, size_t index, const generic_list *item);

typedef void (*list_entry_free_func)(generic_list *entry);

#endif /* GENERIC_LIST_TYPES_H */
