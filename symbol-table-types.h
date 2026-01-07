#ifndef TOY_MAP_SIZE_T_TYPES_H
#define TOY_MAP_SIZE_T_TYPES_H 1

#include "iter-types.h"
#include "bool-types.h"
#include "symbol-table-entry-list-types.h"

/* TODO: dynamic resizing */
#define NUM_BUCKETS 13

typedef struct symbol_table_struct {
    size_t num_items;
    symbol_table_entry_list *buckets[NUM_BUCKETS];
} symbol_table;

typedef item_callback_result (*symbol_table_entry_callback)(void *cookie, symbol_table_entry *entry);
typedef item_callback_result (*const_symbol_table_entry_callback)(void *cookie, const symbol_table_entry *entry);

#endif /* TOY_MAP_SIZE_T_TYPES_H */
