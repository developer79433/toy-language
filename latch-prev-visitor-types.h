#ifndef TOY_LATCH_PREV_VISITOR_TYPES_H
#define TOY_LATCH_PREV_VISITOR_TYPES_H 1

#include "latch-visitor-types.h"

typedef struct latch_prev_visitor_struct {
    latch_visitor latch;
    generic_list *prev;
} latch_prev_visitor;

#endif /* TOY_LATCH_PREV_VISITOR_TYPES_H */
