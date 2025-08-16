#include "block.h"
#include "stmt-list.h"

void block_assert_valid(const toy_block *block)
{
    stmt_list_assert_valid(block->stmts);
}

void block_dump(FILE *f, const toy_block *block)
{
    stmt_list_dump(f, block->stmts);
}
