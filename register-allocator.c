#include <assert.h>

#include "register-allocator.h"
#include "function.h"
#include "ast-visitor.h"
#include "symbol-table.h"
#include "str-list.h"
#include "str.h"
#include "log.h"
#include "block.h"
#include "errors.h"
#include "list-visitor.h"

/* TODO: Can the symbol table(s) be moved out of the AST and into the register allocator? */
typedef struct register_allocator_struct {
    ast_visitor ast_vis;
    toy_block *cur_block;
} register_allocator;

static item_callback_result handle_block(register_allocator *ra, toy_block *block)
{
    toy_block *prev_block = ra->cur_block;
    ra->cur_block = block;
    item_callback_result res = default_block((ast_visitor *) ra, block);
    ra->cur_block = prev_block;
    return res;
}

static size_t add_declaration(toy_block *block, toy_str name)
{
    symbol_table_entry *parameter = symbol_table_get(&block->parameter_symbols, name);
    if (parameter) {
        duplicate_identifier(name);
    }
    symbol_table_entry *decl = symbol_table_get(&block->declaration_symbols, name);
    if (decl) {
        duplicate_identifier(name);
    }
    return symbol_table_add(&block->declaration_symbols, name);
}

static item_callback_result handle_func_decl(register_allocator *ra, toy_func_decl_stmt *func_decl)
{
    assert(ra->cur_block);
    func_decl->decl_index = add_declaration(ra->cur_block, func_decl->func->name);
    return default_func_decl((ast_visitor *) ra, func_decl);
}

static item_callback_result handle_var_decl(register_allocator *ra, toy_var_decl *var_decl)
{
    assert(ra->cur_block);
    var_decl->decl_index = add_declaration(ra->cur_block, var_decl->name);
    return default_var_decl((ast_visitor *) ra, var_decl);
}

typedef struct param_add_visitor_struct {
    list_visitor list_vis;
    symbol_table *parameter_symbols;
} param_add_visitor;

static item_callback_result add_param_to_symbol_table(param_add_visitor *param_add_vis, size_t index, toy_str_list *item)
{
    toy_str param_name = str_list_payload(item);
    symbol_table_add(param_add_vis->parameter_symbols, param_name);
    return CONTINUE_ENUMERATION;
}

static item_callback_result handle_func_expr(register_allocator *ra, toy_function *func)
{
    toy_block *block = func->code;
    symbol_table *parameter_symbols = &block->parameter_symbols;
    param_add_visitor param_add_vis = {
        .list_vis.visit_entry = (list_entry_visit_func) add_param_to_symbol_table,
        .parameter_symbols = parameter_symbols
    };
    enumeration_result res = list_visitor_visit_list((list_visitor *) &param_add_vis, (generic_list *) func->param_names);
    assert(ENUMERATION_COMPLETE == res);
    /* log_printf("regalloc: function %s (%p, block %p) has %zd variables\n", func->name, func, &func->code, symbol_table_size(&func->code.variables)); */
    return default_func_expr((ast_visitor *) ra, func);
}

static register_allocator the_register_allocator = {
    .cur_block = NULL,
    .ast_vis.block = (visit_block_func) handle_block,
    .ast_vis.func_decl = (visit_func_decl_func) handle_func_decl,
    .ast_vis.func_expr = (visit_func_expr_func) handle_func_expr,
    .ast_vis.var_decl = (visit_var_decl_func) handle_var_decl
};

void allocate_registers(toy_function *func)
{
    register_allocator allocator = the_register_allocator;
    item_callback_result res = visit_func_expr((ast_visitor *) &allocator, func);
    assert(CONTINUE_ENUMERATION == res);
}
