#include <assert.h>

#include "block.h"
#include "stmt-list.h"
#include "var-decl-list.h"
#include "log.h"
#include "mymalloc.h"

toy_block toplevel_block = {
    .stmts = NULL,
    .parent = NULL
};

toy_block *block_parent(toy_block *block)
{
    return block->parent;
}

const toy_block *block_parent_const(const toy_block *block)
{
    return block->parent;
}

size_t block_num_declarations(const toy_block *block)
{
    return symbol_table_size(&block->declaration_symbols);
}

size_t block_num_parameters(const toy_block *block)
{
    return symbol_table_size(&block->parameter_symbols);
}

symbol_table *block_parameters(toy_block *block)
{
    return &block->parameter_symbols;
}

symbol_table *block_declarations(toy_block *block)
{
    return &block->declaration_symbols;
}

void block_assert_valid(const toy_block *block)
{
    assert(block);
    assert(block == &toplevel_block || block->parent);
    symbol_table_assert_valid(&block->parameter_symbols);
    symbol_table_assert_valid(&block->declaration_symbols);
    stmt_list_assert_valid(block->stmts);
}

void block_dump(const toy_block *block)
{
    block_assert_valid(block);
    log_debug("block {");
    stmt_list_dump(block->stmts);
    log_debug("}");
}

void block_init(toy_block *block, toy_block *parent, toy_stmt_list *stmt_list)
{
    block->parent = parent;
    block->stmts = stmt_list;
}

toy_block *block_alloc(toy_stmt_list *stmt_list)
{
    toy_block *block = mymalloc(toy_block);
    block_init(block, NULL, stmt_list);
    return block;
}

void block_free(toy_block *block)
{
    stmt_list_free(block->stmts);
    free(block);
}
