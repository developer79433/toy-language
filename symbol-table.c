#include <assert.h>

#include "symbol-table.h"
#include "str.h"
#include "mymalloc.h"
#include "val.h"
#include "map-val.h"
#include "debug.h"

void symbol_table_init(symbol_table *table)
{
    table->symbols = NULL;
    symbol_table_assert_valid(table);
}

void symbol_table_init_ref(symbol_table *table, symbol_table *existing)
{
    table->symbols = existing->symbols;
}

symbol_table *symbol_table_alloc(void)
{
    symbol_table *table = mymalloc(symbol_table);
    symbol_table_init(table);
    symbol_table_assert_valid(table);
    return table;
}

toy_val *symbol_table_get(symbol_table *table, const toy_str name)
{
    symbol_table_assert_valid(table);
    str_assert_valid(name);
    if (table->symbols) {
        return map_val_get(table->symbols, name);
    }
    return NULL;
}

set_result symbol_table_set(symbol_table *table, const toy_str name, const toy_val *value)
{
    symbol_table_assert_valid(table);
    str_assert_valid(name);
    val_assert_valid(value);
    if (!table->symbols) {
        table->symbols = map_val_alloc();
    }
    assert(table->symbols);
    set_result res = map_val_set(table->symbols, name, (toy_val *) value);
    return res;
}

symbol_table *symbol_table_ref(symbol_table *table)
{
    symbol_table_assert_valid(table);
    return table;
}

void symbol_table_free(symbol_table *table)
{
    symbol_table_assert_valid(table);
    if (table->symbols) {
        map_val_assert_valid(table->symbols);
        map_val_free(table->symbols);
        table->symbols = NULL;
    }
    // This is no longer separately allocated
    // free(table);
}

#ifndef NDEBUG

void symbol_table_assert_valid(const symbol_table *table)
{
    assert(table);
    if (table->symbols) {
        if (valid_check_depth < VALID_CHECK_RECURSION_DEPTH) {
            valid_check_depth++;
            map_val_assert_valid(table->symbols);
            valid_check_depth--;
        }
    }
}

#endif /* ndef NDEBUG */

void symbol_table_dump(const symbol_table *table)
{
    symbol_table_assert_valid(table);
    if (table->symbols) {
        map_val_assert_valid(table->symbols);
        map_val_dump(table->symbols);
    }
}
