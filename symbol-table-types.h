#ifndef TOY_SYMBOL_TABLE_TYPES_H
#define TOY_SYMBOL_TABLE_TYPES_H 1

#include <stddef.h>

#include "map-val-types.h"

typedef struct symbol_table_struct {
    map_val *symbols;
} symbol_table;

#endif /* TOY_SYMBOL_TABLE_TYPES_H */
