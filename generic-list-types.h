#ifndef GENERIC_LIST_TYPES_H
#define GENERIC_LIST_TYPES_H 1

#include <stddef.h>
#include <inttypes.h>

#include "bool-types.h"
#include "iter-types.h"

typedef struct generic_list_struct {
    struct generic_list_struct *next;
} generic_list;

typedef item_callback_result (*generic_list_item_callback)(void *cookie, size_t index, generic_list *item);
typedef item_callback_result (*const_generic_list_item_callback)(void *cookie, size_t index, const generic_list *item);

extern void *INDEX_OUT_OF_BOUNDS;

typedef toy_bool (*generic_list_filter_func)(void *cookie, size_t index, const generic_list *item);

typedef struct filter_args_struct {
    generic_list_filter_func filter;
    void *filter_cookie;
    generic_list_item_callback user_callback;
    void *user_cookie;
} filter_args;

#endif /* GENERIC_LIST_TYPES_H */
