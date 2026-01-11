#ifndef TOY_BLOCK_H
#define TOY_BLOCK_H 1

#include <stddef.h>

#include "block-types.h"
#include "symbol-table-types.h"

extern toy_block toplevel_block;

void block_init(toy_block *block, toy_block *parent, toy_stmt_list *stmt_list);
size_t block_num_declarations(const toy_block *block);
size_t block_num_parameters(const toy_block *block);
symbol_table *block_parameters(toy_block *block);
symbol_table *block_declarations(toy_block *block);
void block_assert_valid(const toy_block *block);
void block_dump(const toy_block *block);
toy_block *block_parent(toy_block *block);
const toy_block *block_parent_const(const toy_block *block);
toy_block *block_alloc(toy_stmt_list *stmt_list);
void block_free(toy_block *block);

#endif /* TOY_BLOCK_H */
