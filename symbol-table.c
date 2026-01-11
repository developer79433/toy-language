#include <assert.h>

#include "symbol-table.h"
#include "map-buf.h"
#include "str.h"
#include "mymalloc.h"
#include "debug.h"

void symbol_table_init(symbol_table *table)
{
    symbol_table_assert_valid(table);
}

symbol_table *symbol_table_alloc(void)
{
    symbol_table *table = mymalloc(symbol_table);
    symbol_table_init(table);
    symbol_table_assert_valid(table);
    return table;
}

size_t symbol_table_size(const symbol_table *table)
{
    return map_buf_size((const map_buf *) table);
}

symbol_table_entry *symbol_table_get(symbol_table *table, const toy_str name)
{
    symbol_table_assert_valid(table);
    str_assert_valid(name);
    return map_buf_get((map_buf *) table, name);
}

const symbol_table_entry *symbol_table_get_const(const symbol_table *table, const toy_str name)
{
    symbol_table_assert_valid(table);
    str_assert_valid(name);
    return map_buf_get_const((const map_buf *) table, name);
}

set_result symbol_table_set(symbol_table *table, const toy_str name, symbol_table_entry *entry)
{
    symbol_table_assert_valid(table);
    str_assert_valid(name);
    return map_buf_set((map_buf *) table, name, entry, sizeof(*entry));
}

size_t symbol_table_add(symbol_table *table, const toy_str name)
{
    symbol_table_assert_valid(table);
    str_assert_valid(name);
    size_t num_variables = symbol_table_size(table);
    symbol_table_entry entry = { .index = num_variables, .key = name };
    set_result res = map_buf_set((map_buf *) table, name, &entry, sizeof(entry));
    assert(SET_NEW == res);
    return num_variables;
}

void symbol_table_free(symbol_table *table)
{
    symbol_table_assert_valid(table);
    map_buf_free((map_buf *) table);
}

#ifndef NDEBUG

void symbol_table_assert_valid(const symbol_table *table)
{
    assert(table);
    map_buf_assert_valid((map_buf *) table);
}

#endif /* ndef NDEBUG */

void symbol_table_dump(const symbol_table *table)
{
    symbol_table_assert_valid(table);
    /* TODO */
}

enumeration_result symbol_table_foreach(symbol_table *table, symbol_table_entry_callback callback, void *cookie)
{
    return map_buf_foreach((map_buf *) table, (map_buf_entry_callback) callback, cookie);
}

enumeration_result symbol_table_foreach_const(const symbol_table *table, const_symbol_table_entry_callback callback, void *cookie)
{
    return map_buf_foreach_const((const map_buf *) table, (const_map_buf_entry_callback) callback, cookie);
}
