#include <assert.h>

#include "symbol-table.h"
#include "str.h"
#include "mymalloc.h"
#include "map-size-t.h"
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

size_t *symbol_table_get(symbol_table *table, const toy_str name)
{
    symbol_table_assert_valid(table);
    str_assert_valid(name);
    return map_size_t_get(table, name);
}

const size_t *symbol_table_get_const(const symbol_table *table, const toy_str name)
{
    symbol_table_assert_valid(table);
    str_assert_valid(name);
    return map_size_t_get_const(table, name);
}

size_t symbol_table_set(symbol_table *table, const toy_str name)
{
    symbol_table_assert_valid(table);
    str_assert_valid(name);
    size_t num_variables = map_size_t_size(table);
    set_result set_res = map_size_t_set(table, name, num_variables);
    assert(SET_NEW == set_res);
    return num_variables;
}

void symbol_table_free(symbol_table *table)
{
    symbol_table_assert_valid(table);
    map_size_t_free(table);
}

#ifndef NDEBUG

void symbol_table_assert_valid(const symbol_table *table)
{
    assert(table);
    map_size_t_assert_valid(table);
}

#endif /* ndef NDEBUG */

void symbol_table_dump(const symbol_table *table)
{
    symbol_table_assert_valid(table);
    map_size_t_dump(table);
}
