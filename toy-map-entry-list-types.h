#ifndef TOY_MAP_ENTRY_LIST_TYPES_H
#define TOY_MAP_ENTRY_LIST_TYPES_H 1

#include "toy-str-types.h"

struct toy_expr_struct;
typedef struct toy_expr_struct toy_expr;

typedef struct toy_map_entry_list_struct {
    struct toy_map_entry_list_struct *next;
    toy_str key;
    toy_expr *value;
} toy_map_entry_list;

#endif /* TOY_MAP_ENTRY_LIST_TYPES_H */
