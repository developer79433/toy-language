#ifndef TOY_MAP_ENTRY_TYPES_H
#define TOY_MAP_ENTRY_TYPES_H 1

#include "str-types.h"

struct toy_expr_struct;
typedef struct toy_expr_struct toy_expr;

typedef struct toy_map_entry_struct {
    toy_str key;
    toy_expr *value;
} toy_map_entry;

#endif /* TOY_MAP_ENTRY_TYPES_H */
