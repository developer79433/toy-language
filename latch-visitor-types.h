#ifndef TOY_LATCH_VISITOR_TYPES_H
#define TOY_LATCH_VISITOR_TYPES_H 1

#include "generic-list-types.h"

typedef struct latch_visitor_struct {
    list_visitor base;
    generic_list *last_seen_item;
    toy_bool stop_on_first;
} latch_visitor;

#endif /* TOY_LATCH_VISITOR_TYPES_H */
