#ifndef TOY_MAP_EXPR_ENTRY_LIST_TYPES_H
#define TOY_MAP_EXPR_ENTRY_LIST_TYPES_H 1

#include <stddef.h>

#include "iter-types.h"
#include "map-expr-entry-types.h"

typedef struct toy_map_expr_entry_list_struct {
    struct toy_map_expr_entry_list_struct *next;
    toy_map_expr_entry entry;
} toy_map_expr_entry_list;

#endif /* TOY_MAP_EXPR_ENTRY_LIST_TYPES_H */
