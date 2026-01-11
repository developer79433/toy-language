#ifndef TOY_SYMBOL_TABLE_ENTRY_LIST_TYPES_H
#define TOY_SYMBOL_TABLE_ENTRY_LIST_TYPES_H 1

#include <stddef.h>

#include "bool-types.h"
#include "str-types.h"

typedef struct symbol_table_entry_struct {
    toy_str key;
    size_t index;
} symbol_table_entry;

typedef struct symbol_table_entry_list_struct {
    struct symbol_table_entry_list_struct *next;
    symbol_table_entry entry;
} symbol_table_entry_list;

#endif /* TOY_SYMBOL_TABLE_ENTRY_LIST_TYPES_H */
