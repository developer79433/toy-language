#include <assert.h>

#include "register-allocator.h"
#include "function.h"
#include "visitor.h"
#include "symbol-table.h"

static item_callback_result symtab_entry_cb(void *cookie, symbol_table_entry *entry)
{
    return CONTINUE_ENUMERATION;
}

void handle_block(visitor *v, toy_block *block)
{
    enumeration_result res = symbol_table_foreach(&block->variables, symtab_entry_cb, NULL);
    assert(ENUMERATION_COMPLETE == res);
    default_block(v, block);
}

static visitor register_allocator = {
    .block = handle_block
};

void allocate_registers(toy_function *func)
{
    visit_func_expr(&register_allocator, func);
}
