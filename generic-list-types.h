#ifndef GENERIC_LIST_TYPES_H
#define GENERIC_LIST_TYPES_H 1

#include <stddef.h>
#include <inttypes.h>

typedef struct generic_list_struct {
    struct generic_list_struct *next;
    /* TODO: Keep track of length? */
} generic_list;

typedef enum listitem_callback_result_enum {
    STOP_ITERATING = 0,
    KEEP_ITERATING = 1
} listitem_callback_result;

typedef enum list_iter_result_enum {
    REACHED_END = 0,
    CALLBACK_STOPPED = 1
} list_iter_result;

typedef listitem_callback_result (*generic_list_item_callback)(void *cookie, size_t index, generic_list *item);
typedef listitem_callback_result (*const_generic_list_item_callback)(void *cookie, size_t index, const generic_list *item);

extern void *INDEX_OUT_OF_BOUNDS;

#endif /* GENERIC_LIST_TYPES_H */
