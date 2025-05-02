#ifndef GENERIC_LIST_TYPES_H
#define GENERIC_LIST_TYPES_H 1

#include <stddef.h>
#include <inttypes.h>

#include "bool-types.h"

typedef struct generic_list_struct {
    struct generic_list_struct *next;
} generic_list;

typedef enum listitem_callback_result_enum {
    STOP_ITERATING = 0,
    CONTINUE_ITERATING = 1
} listitem_callback_result;

typedef enum list_iter_result_enum {
    ENUMERATION_COMPLETE = 0,
    EUMERATION_INTERRUPTED = 1
} list_iter_result;

typedef listitem_callback_result (*generic_list_item_callback)(void *cookie, size_t index, generic_list *item);
typedef listitem_callback_result (*const_generic_list_item_callback)(void *cookie, size_t index, const generic_list *item);

extern void *INDEX_OUT_OF_BOUNDS;

typedef toy_bool (*generic_list_filter_func)(void *cookie, size_t index, generic_list *item);

typedef struct filter_args_struct {
    generic_list_filter_func filter;
    generic_list_item_callback user_callback;
    void *user_cookie;
} filter_args;

#endif /* GENERIC_LIST_TYPES_H */
