#include <assert.h>
#include <string.h>

#include "name-resolver.h"
#include "expr-types.h"
#include "resolved-name.h"
#include "constants.h"
#include "predef-function.h"
#include "errors.h"
#include "ast-visitor.h"
#include "var-decl.h"

#if 0
#define DEBUG_NAME_RESOLUTION
#endif

typedef struct name_resolver_struct {
    ast_visitor ast_vis;
    toy_block *cur_block;
} name_resolver;

typedef enum decl_ref_type_enum {
    DECL_REF_VAR,
    DECL_REF_FUNC
} decl_ref_type;

typedef struct decl_ref_struct {
    decl_ref_type type;
    union {
        toy_var_decl_stmt *var_decl;
        toy_func_decl_stmt *func_decl;
    };
} decl_ref;

static void resolve_identifier_block_and_parents(toy_block *block, toy_identifier *identifier)
{
    size_t frames_up = 0;
    do {
        symbol_table_entry *entry;
        /* FIXME: Never sets REF_FUNC_DECL */
        /* TODO: Just put all the declarations into an array already! */
        symbol_table_assert_valid(&block->declaration_symbols);
        entry = symbol_table_get(&block->declaration_symbols, identifier->name);
        if (entry) {
            identifier->resolved.type = REF_VAR_DECL;
            identifier->resolved.var_decl.frames_up = frames_up;
            identifier->resolved.var_decl.var_index = entry->index;
#ifdef DEBUG_NAME_RESOLUTION
            log_printf("Resolved '%s' to variable %d, %d frames up\n", identifier->name, *i, frames_up);
#endif /* DEBUG_NAME_RESOLUTION */
            return;
        }
        symbol_table_assert_valid(&block->parameter_symbols);
        entry = symbol_table_get(&block->parameter_symbols, identifier->name);
        if (entry) {
            identifier->resolved.type = REF_FUNC_PARAM;
            identifier->resolved.func_param.frames_up = frames_up;
            identifier->resolved.func_param.var_index = entry->index;
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

static item_callback_result handle_block(name_resolver *nr, toy_block *block)
{
#ifdef DEBUG_NAME_RESOLUTION
    log_printf("*** BLOCK\n");
#endif /* DEBUG_NAME_RESOLUTION */
    toy_block *prev_block = nr->cur_block;
    nr->cur_block = block;
    item_callback_result res = default_block((ast_visitor *) nr, block);
    nr->cur_block = prev_block;
    return res;
}

static item_callback_result handle_identifier(name_resolver *nr, toy_identifier *identifier)
{
    assert(nr->cur_block);
#ifdef DEBUG_NAME_RESOLUTION
    log_printf("*** IDENTIFIER: %s\n", identifier->name);
#endif /* DEBUG_NAME_RESOLUTION */
    resolve_identifier(nr->cur_block, identifier);
    return default_identifier((ast_visitor *) nr, identifier);
}

static name_resolver the_name_resolver = {
    .ast_vis.block = (visit_block_func) handle_block,
    .ast_vis.identifier = (visit_identifier_func) handle_identifier,
    .cur_block = NULL
};

void resolve_names(toy_function *func)
{
    visit_func_expr((ast_visitor *) &the_name_resolver, func);
}
