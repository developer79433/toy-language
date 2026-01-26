
#include <assert.h>

#include "ast-visitor.h"
#include "log.h"
#include "block.h"

toy_bool parent_adder_ran = TOY_FALSE;

typedef struct block_parent_adder_struct {
    ast_visitor ast_vis;
    toy_block *cur_block;
} block_parent_adder;

static item_callback_result handle_block(block_parent_adder *parent_adder, toy_block *block)
{
#ifdef DEBUG_BLOCK_PARENTS
    log_printf("**** Push block\n");
    log_printf("  ** Cur block:\n");
    if (cur_block) {
        block_dump(cur_block);
    }
    log_printf("  ** New block:\n");
    block_dump(block);
#endif /* DEBUG_BLOCK_PARENTS */
    block->parent = parent_adder->cur_block;
    toy_block *prev_block = parent_adder->cur_block;
    parent_adder->cur_block = block;
    item_callback_result res = default_block(&parent_adder->ast_vis, block);
#ifdef DEBUG_BLOCK_PARENTS
    log_printf("**** Pop block\n");
    log_printf("  ** Cur block:\n");
    block_dump(cur_block);
    log_printf("  ** Old block:\n");
    if (backup_cur_block) {
        block_dump(backup_cur_block);
    }
#endif /* DEBUG_BLOCK_PARENTS */
    parent_adder->cur_block = prev_block;
    return res;
}

static block_parent_adder the_block_parent_adder = { .ast_vis.block = (visit_block_func) handle_block, .cur_block = NULL };

void add_block_parents(toy_function *func)
{
    block_parent_adder parent_adder = the_block_parent_adder;
    item_callback_result res = visit_func_expr((ast_visitor *) &parent_adder, func);
    assert(CONTINUE_ENUMERATION == res);
    parent_adder_ran = TOY_TRUE;
}
