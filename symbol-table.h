#ifndef TOY_SYMBOL_TABLE_H
#define TOY_SYMBOL_TABLE_H 1

#include <stdio.h>

#include "symbol-table-types.h"
#include "str-types.h"
#include "val-types.h"
#include "iter-types.h"

void symbol_table_init(symbol_table *table);
void symbol_table_init_ref(symbol_table *table, symbol_table *existing);
symbol_table *symbol_table_alloc(void);
toy_val *symbol_table_get(symbol_table *table, const toy_str name);
set_result symbol_table_set(symbol_table *table, const toy_str name, const toy_val *value);
void symbol_table_dump(FILE *f, const symbol_table *table);
#ifdef NDEBUG
#define symbol_table_assert_valid(table) do {} while (0)
#else /* ndef NDEBUG */
void symbol_table_assert_valid(const symbol_table *table);
#endif /* ndef NDEBUG */
symbol_table *symbol_table_ref(symbol_table *table);
void symbol_table_free(symbol_table *table);

#endif /* TOY_SYMBOL_TABLE_H */
