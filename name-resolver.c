#include <assert.h>
#include <string.h>

#include "name-resolver.h"
#include "expr-types.h"
#include "resolved-name.h"
#include "constants.h"
#include "predef-function.h"
#include "errors.h"
#include "visitor.h"
#include "var-decl.h"

#if 0
#define DEBUG_NAME_RESOLUTION
#endif

static void resolve_identifier_block_and_parents(toy_block *block, toy_identifier *identifier)
{
    size_t frames_up = 0;
    do {
        symbol_table_entry *entry;
        symbol_table_assert_valid(&block->variables);
        entry = symbol_table_get(&block->variables, identifier->name);
        if (entry) {
            identifier->resolved.type = REF_VAR_DECL;
            identifier->resolved.var_decl.frames_up = frames_up;
            identifier->resolved.var_decl.var_index = entry->index;
            if (frames_up != 0 && !entry->is_closed_over) {
                entry->is_closed_over = TOY_TRUE;
                symbol_table_set(&block->variables, identifier->name, entry);
            }
#ifdef DEBUG_NAME_RESOLUTION
            log_printf("Resolved '%s' to variable %d, %d frames up\n", identifier->name, *i, frames_up);
#endif /* DEBUG_NAME_RESOLUTION */
            return;
        }
        symbol_table_assert_valid(&block->parameters);
        entry = symbol_table_get(&block->parameters, identifier->name);
        if (entry) {
            identifier->resolved.type = REF_FUNC_PARAM;
            identifier->resolved.func_param.frames_up = frames_up;
            identifier->resolved.func_param.param_index = entry->index;
            if (frames_up != 0 && !entry->is_closed_over) {
                entry->is_closed_over = TOY_TRUE;
                symbol_table_set(&block->parameters, identifier->name, entry);
            }
#ifdef DEBUG_NAME_RESOLUTION
            log_printf("Resolved '%s' to function parameter %d, %d frames up\n", identifier->name, *i, frames_up);
#endif /* DEBUG_NAME_RESOLUTION */
            return;
        }
        block = block->parent;
        frames_up++;
    } while (block);
}

static void resolve_identifier(toy_block *block, toy_identifier *identifier)
{
#ifdef DEBUG_NAME_RESOLUTION
    log_printf("Resolving identifier '%s'\n", identifier->name);
#endif /* DEBUG_NAME_RESOLUTION */
    resolve_identifier_block_and_parents(block, identifier);
    if (!is_resolved(&identifier->resolved)) {
        const predefined_constant *predef_const = lookup_predefined_constant(identifier->name);
        if (predef_const) {
            identifier->resolved.type = REF_PREDEF_CONST;
            identifier->resolved.predef_const = predef_const;
#ifdef DEBUG_NAME_RESOLUTION
            log_printf("Resolved '%s' to predef constant\n", identifier->name);
#endif /* DEBUG_NAME_RESOLUTION */
        }
    }
    if (!is_resolved(&identifier->resolved)) {
        const toy_val *val = predef_func_lookup_name(identifier->name);
        if (val) {
            assert(VAL_FUNC == val->type);
            const toy_function *func = val->func;
            assert(FUNC_PREDEFINED == func->type);
            identifier->resolved.type = REF_PREDEF_FUNC;
            identifier->resolved.predef_func = val;
#ifdef DEBUG_NAME_RESOLUTION
            log_printf("Resolved '%s' to predef function\n", identifier->name);
#endif /* DEBUG_NAME_RESOLUTION */
        }
    }
    if (!is_resolved(&identifier->resolved)) {
        undeclared_identifier(identifier->name);
    }
}

static toy_block *cur_block = NULL;

static void handle_block(visitor *v, toy_block *block)
{
#ifdef DEBUG_NAME_RESOLUTION
    log_printf("*** BLOCK\n");
#endif /* DEBUG_NAME_RESOLUTION */
    toy_block *old_block = cur_block;
    cur_block = block;
    symbol_table_init(&block->variables);
    symbol_table_init(&block->parameters);
    default_block(v, block);
    cur_block = old_block;
}

static toy_func_decl_stmt *cur_func_decl = NULL;

static void handle_func_decl(visitor *v, toy_func_decl_stmt *func_decl)
{
#ifdef DEBUG_NAME_RESOLUTION
    log_printf("*** FUNC DECL: %s\n", func_decl->func.name);
#endif /* DEBUG_NAME_RESOLUTION */
    toy_func_decl_stmt *old_func_decl = cur_func_decl;
    cur_func_decl = func_decl;
    assert(cur_block);
    symbol_table_assert_valid(&cur_block->variables);
    symbol_table_add(&cur_block->variables, func_decl->func.name);
    default_func_decl(v, func_decl);
    cur_func_decl = old_func_decl;
}

static toy_var_decl *cur_var_decl = NULL;

static void handle_var_decl(visitor *v, toy_var_decl *var_decl)
{
#ifdef DEBUG_NAME_RESOLUTION
    log_printf("*** VAR DECL: %s\n", var_decl->name);
#endif /* DEBUG_NAME_RESOLUTION */
    toy_var_decl *old_var_decl = cur_var_decl;
    cur_var_decl = var_decl;
    var_decl_assert_valid(var_decl);
    assert(cur_block);
    symbol_table_assert_valid(&cur_block->variables);
    symbol_table_add(&cur_block->variables, var_decl->name);
    default_var_decl(v, var_decl);
    cur_var_decl = old_var_decl;
}

static toy_function *cur_func_expr = NULL;

static void handle_func_expr(visitor *v, toy_function *func)
{
#ifdef DEBUG_NAME_RESOLUTION
    log_printf("*** FUNC EXPR\n");
#endif /* DEBUG_NAME_RESOLUTION */
    toy_function *old_func_expr = cur_func_expr;
    cur_func_expr = func;
    default_func_expr(v, func);
    cur_func_expr = old_func_expr;
}

static void handle_parameter(visitor *v, toy_str parameter)
{
#ifdef DEBUG_NAME_RESOLUTION
    log_printf("*** PARAM: %s\n", parameter);
#endif /* DEBUG_NAME_RESOLUTION */
    assert(cur_func_decl || cur_func_expr);
    assert(cur_block);
    symbol_table_assert_valid(&cur_block->parameters);
    symbol_table_add(&cur_block->parameters, parameter);
    default_parameter(v, parameter);
}

static void handle_identifier(visitor *v, toy_identifier *identifier)
{
    assert(cur_block);
#ifdef DEBUG_NAME_RESOLUTION
    log_printf("*** IDENTIFIER: %s\n", identifier->name);
#endif /* DEBUG_NAME_RESOLUTION */
    resolve_identifier(cur_block, identifier);
    default_identifier(v, identifier);
}

static visitor resolver = {
    .block = handle_block,
    .func_decl = handle_func_decl,
    .func_expr = handle_func_expr,
    .identifier = handle_identifier,
    .parameter = handle_parameter,
    .var_decl = handle_var_decl
};

void resolve_names(toy_function *func)
{
    visit_func_expr(&resolver, func);
}
