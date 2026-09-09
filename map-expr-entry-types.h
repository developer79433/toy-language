#ifndef TOY_MAP_EXPR_ENTRY_TYPES_H
#define TOY_MAP_EXPR_ENTRY_TYPES_H 1

#include <stddef.h>

#include "str-types.h"

struct toy_expr_struct;
typedef struct toy_expr_struct toy_expr;

typedef struct toy_map_expr_entry_struct {
    void *key;
    size_t key_len;
    toy_expr *expr;
} toy_map_expr_entry;

#endif /* TOY_MAP_EXPR_ENTRY_TYPES_H */
