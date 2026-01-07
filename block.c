#include <assert.h>

#include "block.h"
#include "stmt-list.h"
#include "var-decl-list.h"
#include "log.h"

toy_block *block_parent(toy_block *block)
{
    return block->parent;
}

const toy_block *block_parent_const(const toy_block *block)
{
    return block->parent;
}

size_t block_num_variables(const toy_block *block)
{
    return symbol_table_size(&block->variables);
}

size_t block_num_parameters(const toy_block *block)
{
    return symbol_table_size(&block->parameters);
}

void block_assert_valid(const toy_block *block)
{
    assert(block);
    stmt_list_assert_valid(block->stmts);
    /* TODO */
    /* block_assert_valid(block->parent); */
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
    block_assert_valid(block);
}

typedef struct count_variables_cb_args_struct {
    size_t num_variables;
} count_variables_cb_args;

static item_callback_result count_variables_callback(void *cookie, size_t index, toy_stmt_list *item)
{
    count_variables_cb_args *args = (count_variables_cb_args *) cookie;
    toy_stmt *stmt = stmt_list_payload(item);
    switch(stmt->type) {
    case STMT_FUNC_DECL:
        args->num_variables++;
        break;
    case STMT_VAR_DECL:
        toy_var_decl_list *var_decl_list = &stmt->var_decl_stmt;
        args->num_variables += var_decl_list_len(var_decl_list);
        break;
    default:
        break;
    }
    return CONTINUE_ENUMERATION;
}

size_t count_variables(const toy_block *block)
{
    block_assert_valid(block);
    count_variables_cb_args args = { .num_variables = 0 };
    enumeration_result res = stmt_list_foreach(block->stmts, count_variables_callback, &args);
    assert(ENUMERATION_COMPLETE == res);
    return args.num_variables;
}
