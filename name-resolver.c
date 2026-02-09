#include <assert.h>
#include <string.h>

#include "name-resolver.h"
#include "expr-types.h"
#include "constant.h"
#include "predef-function.h"
#include "errors.h"
#include "ast-visitor.h"
#include "var-decl.h"
#include "stmt.h"
#include "generic-list.h"
#include "stmt-list.h"
#include "log.h"
#include "rev-list-subset-visitor.h"
#include "stmt-list.h"
#include "list-visitor.h"
#include "block.h"
#include "str.h"
#include "decl-ref-list.h"
#include "decl-ref.h"
#include "str-list.h"
#include "mymalloc.h"

#if 0
#define DEBUG_NAME_RESOLUTION
#endif

typedef struct name_resolver_struct {
    ast_visitor ast_vis;
    toy_block *cur_block;
    size_t symbol_index;
} name_resolver;

static void resolve_identifier_block_and_parents(name_resolver *resolver, toy_identifier *identifier)
{
    size_t frames_up = 0;
    toy_block *block = resolver->cur_block;
    do {
        identifier->decl = block_resolve_identifier(block, identifier->name);
        if (identifier->decl) {
            identifier->decl->frames_up = frames_up;
            identifier->decl->block = block;
            return;
        }
        block = block_parent(block);
        frames_up++;
    } while (block);
}

static void resolve_identifier(name_resolver *resolver, toy_identifier *identifier)
{
#ifdef DEBUG_NAME_RESOLUTION
    log_debug("Resolving identifier '%s'\n", identifier->name);
#endif /* DEBUG_NAME_RESOLUTION */
    resolve_identifier_block_and_parents(resolver, identifier);
    if (identifier->decl) {
        decl_ref *ref = identifier->decl;
#ifdef DEBUG_NAME_RESOLUTION
        log_debug("Resolved '%s' to %s\n", identifier->name, decl_ref_type_name(ref->type));
#endif /* DEBUG_NAME_RESOLUTION */
        if (ref->frames_up != 0) {
            switch (ref->type) {
            case DECL_REF_FUNC:
                /* Function declarations are immutable, so no need for a closure */
                break;
            case DECL_REF_PARAM:
                /* Closure over a function parameter in an enclosing block */
                func_param_ref *param_ref = &ref->func_param;
                log_debug_file("Closure over parameter #%zu to function %s, %zu frames up\n", param_ref->param_index, param_ref->func->name, ref->frames_up);
                break;
            case DECL_REF_PREDEF:
                /* Predefined constant and functions are immutable, so no need for a closure */
                break;
            case DECL_REF_VAR:
                /* Closure over a variable in an enclosing block */
                toy_var_decl *var_decl = ref->var_decl;
                log_debug_file("Closure over variable %s, %zu frames up\n", var_decl->name, ref->frames_up);
                break;
            default:
                assert(0);
                break;
            }
            resolver->cur_block->num_closures++;
        }
    }
    if (!identifier->decl) {
        const toy_val *val = constant_get_val(identifier->name);
        if (val) {
            identifier->decl = decl_ref_alloc_predef(val);
#ifdef DEBUG_NAME_RESOLUTION
            log_debug_file("Resolved '%s' to predef constant\n", identifier->name);
#endif /* DEBUG_NAME_RESOLUTION */
        }
    }
    if (!identifier->decl) {
        const toy_val *val = predef_func_lookup_name(identifier->name);
        if (val) {
            identifier->decl = decl_ref_alloc_predef(val);
#ifdef DEBUG_NAME_RESOLUTION
            log_debug_file("Resolved '%s' to predef function\n", identifier->name);
#endif /* DEBUG_NAME_RESOLUTION */
        }
    }
    if (!identifier->decl) {
        undeclared_identifier(identifier->name);
    }
}

static item_callback_result handle_func_decl(name_resolver *resolver, toy_func_decl_stmt *func_decl)
{
    block_assert_valid(resolver->cur_block);
    decl_ref ref = { .type = DECL_REF_FUNC, .func_decl = func_decl, .frames_up = 0, .block = resolver->cur_block };
#ifdef DEBUG_NAME_RESOLUTION
    log_debug_file("Adding func declaration:\n");
    decl_ref_dump(&ref);
#endif /* DEBUG_NAME_RESOLUTION */
    block_add_decl_ref(resolver->cur_block, &ref);
    func_decl->decl_index = resolver->symbol_index;
    resolver->symbol_index++;
    return default_func_decl((ast_visitor *) resolver, func_decl);
}

static item_callback_result handle_var_decl(name_resolver *resolver, toy_var_decl *var_decl)
{
    block_assert_valid(resolver->cur_block);
    decl_ref ref = { .type = DECL_REF_VAR, .var_decl = var_decl, .frames_up = 0, .block = resolver->cur_block };
    decl_ref_assert_valid(&ref);
#ifdef DEBUG_NAME_RESOLUTION
    log_debug_file("Adding var declaration:\n");
    decl_ref_dump(&ref);
#endif /* DEBUG_NAME_RESOLUTION */
    block_add_decl_ref(resolver->cur_block, &ref);
    var_decl->decl_index = resolver->symbol_index;
    resolver->symbol_index++;
    return default_var_decl(&resolver->ast_vis, var_decl);
}

typedef struct param_add_visitor_struct {
    list_visitor list_vis;
    toy_function *func;
    name_resolver *resolver;
} param_add_visitor;

static item_callback_result add_param_ref(param_add_visitor *param_add_vis, size_t index, toy_str_list *item)
{
    name_resolver *resolver = param_add_vis->resolver;
    toy_block *cur_block = resolver->cur_block;
    decl_ref ref = {
        .type = DECL_REF_PARAM,
        .func_param.func = param_add_vis->func,
        .func_param.param_index = index,
        .func_param.func = param_add_vis->func,
        .frames_up = 0,
        .block = cur_block
    };
#ifdef DEBUG_NAME_RESOLUTION
    log_debug_file("Adding parameter:\n");
    decl_ref_dump(&ref);
#endif /* DEBUG_NAME_RESOLUTION */
    block_add_decl_ref(cur_block, &ref);
    resolver->symbol_index++;
    return CONTINUE_ENUMERATION;
}

static item_callback_result handle_func_expr(name_resolver *resolver, toy_function *func)
{
    if (resolver->cur_block) {
        block_assert_valid(resolver->cur_block);
        param_add_visitor param_add_vis = {
            .list_vis.visit_entry = (list_entry_visit_func) add_param_ref,
            .func = func,
            .resolver = resolver
        };
        enumeration_result res = list_visitor_visit_list((list_visitor *) &param_add_vis, (generic_list *) func->param_names);
        assert(ENUMERATION_COMPLETE == res);
        /* log_printf("regalloc: function %s (%p, block %p) has %zd variables\n", func->name, func, &func->code, symbol_table_size(&func->code.variables)); */
    } else {
        assert(str_equal(func->name, "global"));
    }
    return default_func_expr(&resolver->ast_vis, func);
}

static item_callback_result handle_block(name_resolver *resolver, toy_block *block)
{
#ifdef DEBUG_NAME_RESOLUTION
    log_debug_file("*** BLOCK\n");
#endif /* DEBUG_NAME_RESOLUTION */
    toy_block *prev_block = resolver->cur_block;
    size_t prev_symbol_index = resolver->symbol_index;
    resolver->cur_block = block;
    resolver->symbol_index = 0;
    item_callback_result res = default_block(&resolver->ast_vis, block);
    resolver->cur_block = prev_block;
    resolver->symbol_index = prev_symbol_index;
    return res;
}

static item_callback_result handle_identifier(name_resolver *resolver, toy_identifier *identifier)
{
    assert(resolver->cur_block);
#ifdef DEBUG_NAME_RESOLUTION
    log_debug_file("*** IDENTIFIER: %s\n", identifier->name);
#endif /* DEBUG_NAME_RESOLUTION */
    resolve_identifier(resolver, identifier);
    return default_identifier(&resolver->ast_vis, identifier);
}

static name_resolver the_name_resolver = {
    .ast_vis.block = (visit_block_func) handle_block,
    .ast_vis.func_decl = (visit_func_decl_func) handle_func_decl,
    .ast_vis.func_expr = (visit_func_expr_func) handle_func_expr,
    .ast_vis.identifier = (visit_identifier_func) handle_identifier,
    .ast_vis.var_decl = (visit_var_decl_func) handle_var_decl,
    .cur_block = NULL,
    .symbol_index = 0
};

void resolve_names(toy_function *func)
{
    name_resolver resolver = the_name_resolver;
    item_callback_result res = visit_func_expr(&resolver.ast_vis, func);
    assert(CONTINUE_ENUMERATION == res);
}
