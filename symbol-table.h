#ifndef TOY_SYMBOL_TABLE_H
#define TOY_SYMBOL_TABLE_H 1

#include <stddef.h>

#include "symbol-table-types.h"
#include "str-types.h"
#include "iter-types.h"

void symbol_table_init(symbol_table *table);
symbol_table *symbol_table_alloc(void);
size_t symbol_table_size(const symbol_table *table);
symbol_table_entry *symbol_table_get(symbol_table *table, const toy_str name);
const symbol_table_entry *symbol_table_get_const(const symbol_table *table, const toy_str name);
set_result symbol_table_set(symbol_table *table, const toy_str name, symbol_table_entry *entry);
size_t symbol_table_add(symbol_table *table, const toy_str name);
void symbol_table_dump(const symbol_table *table);
#ifdef NDEBUG
#define symbol_table_assert_valid(table) do {} while (0)
#else /* ndef NDEBUG */
void symbol_table_assert_valid(const symbol_table *table);
#endif /* ndef NDEBUG */
void symbol_table_free(symbol_table *table);

#endif /* TOY_SYMBOL_TABLE_H */
