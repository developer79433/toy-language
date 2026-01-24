
#include <assert.h>

#include "ast-visitor.h"
#include "log.h"
#include "block.h"

static toy_block *cur_block = NULL;

static item_callback_result handle_block(ast_visitor *v, toy_block *block)
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
    block->parent = cur_block;
    toy_block *prev_block = cur_block;
    cur_block = block;
    item_callback_result res = default_block(v, block);
#ifdef DEBUG_BLOCK_PARENTS
    log_printf("**** Pop block\n");
    log_printf("  ** Cur block:\n");
    block_dump(cur_block);
    log_printf("  ** Old block:\n");
    if (backup_cur_block) {
        block_dump(backup_cur_block);
    }
#endif /* DEBUG_BLOCK_PARENTS */
    cur_block = prev_block;
    return res;
}

static ast_visitor block_parent_adder = { .block = handle_block };

void add_block_parents(toy_function *func)
{
    cur_block = NULL;
    item_callback_result res = visit_func_expr(&block_parent_adder, func);
    assert(CONTINUE_ENUMERATION == res);
}
