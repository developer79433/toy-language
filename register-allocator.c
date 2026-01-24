#include <assert.h>

#include "register-allocator.h"
#include "function.h"
#include "ast-visitor.h"
#include "symbol-table.h"
#include "str-list.h"
#include "str.h"
#include "log.h"
#include "block.h"

static toy_block *cur_block = NULL;

static item_callback_result handle_block(ast_visitor *v, toy_block *block)
{
    toy_block *old_block = cur_block;
    cur_block = block;
    item_callback_result res = default_block(v, block);
    cur_block = old_block;
    return res;
}

static item_callback_result handle_func_decl(ast_visitor *v, toy_func_decl_stmt *func_decl)
{
    assert(cur_block);
    func_decl->decl_index = symbol_table_add(&cur_block->declaration_symbols, func_decl->func->name);
    return default_func_decl(v, func_decl);
}

static item_callback_result handle_var_decl(ast_visitor *v, toy_var_decl *var_decl)
{
    assert(cur_block);
    var_decl->decl_index = symbol_table_add(&cur_block->declaration_symbols, var_decl->name);
    return default_var_decl(v, var_decl);
}

typedef struct add_param_args_struct {
    symbol_table *parameter_symbols;
} add_param_args;

static item_callback_result add_param_to_symbol_table(void *cookie, size_t index, toy_str_list *item)
{
    add_param_args *args = (add_param_args *) cookie;
    toy_str param_name = str_list_payload(item);
    symbol_table_add(args->parameter_symbols, param_name);
    return CONTINUE_ENUMERATION;
}

static item_callback_result handle_func_expr(ast_visitor *v, toy_function *func)
{
    toy_block *block = func->code;
    symbol_table *parameter_symbols = &block->parameter_symbols;
    add_param_args args = { .parameter_symbols = parameter_symbols };
    enumeration_result res = str_list_foreach(func->param_names, add_param_to_symbol_table, &args);
    assert(ENUMERATION_COMPLETE == res);
    /* log_printf("regalloc: function %s (%p, block %p) has %zd variables\n", func->name, func, &func->code, symbol_table_size(&func->code.variables)); */
    return default_func_expr(v, func);
}

static ast_visitor register_allocator = {
    .block = handle_block,
    .func_decl = handle_func_decl,
    .func_expr = handle_func_expr,
    .var_decl = handle_var_decl
};

void allocate_registers(toy_function *func)
{
    visit_func_expr(&register_allocator, func);
}
