#include <assert.h>
#include <stdlib.h>

#include "block.h"
#include "stmt-list.h"
#include "var-decl-list.h"
#include "log.h"
#include "mymalloc.h"
#include "add-block-parents.h"
#include "decl-ref-list.h"

toy_block toplevel_block = {
    .stmts = NULL,
    .parent = NULL
};

toy_block *block_parent(toy_block *block)
{
    return block->parent;
}

size_t block_num_closures(const toy_block *block)
{
    return block->num_closures;
}

const toy_block *block_parent_const(const toy_block *block)
{
    return block->parent;
}

void block_assert_valid(const toy_block *block)
{
    assert(block);
    assert(block == &toplevel_block || block->parent || !parent_adder_ran);
    assert(block->num_closures >= 0);
    stmt_list_assert_valid(block->stmts);
    decl_ref_list_assert_valid(block->decls_rev);
}

void block_dump(const toy_block *block)
{
    log_debug("block { num_closures=%zu, ", block->num_closures);
    stmt_list_dump(block->stmts);
    decl_ref_list_dump(block->decls_rev);
    log_debug("}");
}

void block_init(toy_block *block, toy_block *parent, toy_stmt_list *stmt_list)
{
    block->parent = parent;
    block->stmts = stmt_list;
    block->decls_rev = NULL;
    block->num_closures = 0;
    block_assert_valid(block);
}

toy_block *block_alloc(toy_stmt_list *stmt_list)
{
    toy_block *block = mymalloc(toy_block);
    block_init(block, NULL, stmt_list);
    block_assert_valid(block);
    return block;
}

void block_free(toy_block *block)
{
    stmt_list_free(block->stmts);
    block->stmts = NULL;
    /* TODO: Free decls_rev */
    free(block);
}
