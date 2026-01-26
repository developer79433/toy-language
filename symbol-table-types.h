#ifndef TOY_MAP_SIZE_T_TYPES_H
#define TOY_MAP_SIZE_T_TYPES_H 1

#include "iter-types.h"
#include "bool-types.h"
#include "symbol-table-entry-list-types.h"

typedef struct symbol_table_struct {
    size_t num_items;
    size_t num_buckets;
    symbol_table_entry_list **buckets;
} symbol_table;

#endif /* TOY_MAP_SIZE_T_TYPES_H */
