#ifndef TOY_LIST_LATCH_PREV_TYPES_H
#define TOY_LIST_LATCH_PREV_TYPES_H 1

#include "list-latch-types.h"

typedef struct list_latch_prev_struct {
    list_latch latch;
    generic_list *prev;
} list_latch_prev;

#endif /* TOY_LIST_LATCH_PREV_TYPES_H */
