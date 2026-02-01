#ifndef TOY_LIST_LATCH_TYPES_H
#define TOY_LIST_LATCH_TYPES_H 1

#include "generic-list-types.h"
#include "list-visitor-types.h"

typedef struct list_latch_struct {
    list_visitor visitor;
    generic_list *last_seen_item;
} list_latch;

typedef struct const_list_latch_struct {
    const_list_visitor visitor;
    const generic_list *last_seen_item;
} const_list_latch;

#endif /* TOY_LIST_LATCH_TYPES_H */
