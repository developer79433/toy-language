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
    size_t frames_up;
} name_resolver;

static void add_decl_ref(toy_block *block, decl_ref *ref)
{
    block_assert_valid(block);
    decl_ref_assert_valid(ref);
#ifdef DEBUG_NAME_RESOLUTION
    log_debug_file("Adding declaration:\n");
    decl_ref_dump(ref);
#endif /* DEBUG_NAME_RESOLUTION */
    if (block->decls_rev) {
        block->decls_rev = decl_ref_list_prepend(block->decls_rev, ref);
    } else {
        block->decls_rev = decl_ref_list_alloc(ref);
    }
}

static void resolve_identifier_block(toy_block *block, toy_identifier *identifier)
{
    identifier->decl = decl_ref_list_find_name(block->decls_rev, identifier->name);
    if (identifier->decl) {
        decl_ref_assert_valid(identifier->decl);
    }
}

static void resolve_identifier_block_and_parents(name_resolver *resolver, toy_identifier *identifier)
{
    resolver->frames_up = 0;
    toy_block *block = resolver->cur_block;
    do {
        resolve_identifier_block(block, identifier);
        if (identifier->decl) {
            return;
        }
        block = block->parent;
        resolver->frames_up++;
    } while (block);
}

static void resolve_identifier(name_resolver *resolver, toy_identifier *identifier)
{
#ifdef DEBUG_NAME_RESOLUTION
    log_debug("Resolving identifier '%s'\n", identifier->name);
#endif /* DEBUG_NAME_RESOLUTION */
    resolve_identifier_block_and_parents(resolver, identifier);
    if (identifier->decl) {
#ifdef DEBUG_NAME_RESOLUTION
        log_debug("Resolved '%s' to %s\n", identifier->name, decl_ref_type_name(ref->type));
#endif /* DEBUG_NAME_RESOLUTION */
        if (resolver->frames_up != 0) {
            resolver->cur_block->num_closures++;
        }
    }
    if (!identifier->decl) {
        const toy_val *val = constant_get_val(identifier->name);
        if (val) {
            decl_ref *ref = identifier->decl = mymalloc(decl_ref);
            ref->type = DECL_REF_PREDEF;
            ref->predef = val;
#ifdef DEBUG_NAME_RESOLUTION
            log_debug_file("Resolved '%s' to predef constant\n", identifier->name);
#endif /* DEBUG_NAME_RESOLUTION */
        }
    }
    if (!identifier->decl) {
        const toy_val *val = predef_func_lookup_name(identifier->name);
        if (val) {
            decl_ref *ref = identifier->decl = mymalloc(decl_ref);
            ref->type = DECL_REF_PREDEF;
            ref->predef = val;
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
    decl_ref ref = { .type = DECL_REF_FUNC, .func_decl = func_decl };
    add_decl_ref(resolver->cur_block, &ref);
    func_decl->decl_index = resolver->symbol_index;
    resolver->symbol_index++;
    return default_func_decl((ast_visitor *) resolver, func_decl);
}

static item_callback_result handle_var_decl(name_resolver *resolver, toy_var_decl *var_decl)
{
    block_assert_valid(resolver->cur_block);
    decl_ref ref = { .type = DECL_REF_VAR, .var_decl = var_decl };
    decl_ref_assert_valid(&ref);
    add_decl_ref(resolver->cur_block, &ref);
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
    decl_ref ref = {
        .type = DECL_REF_PARAM,
        .func_param.func = param_add_vis->func,
        .func_param.param_index = index,
        .func_param.func = param_add_vis->func
    };
    name_resolver *resolver = param_add_vis->resolver;
    toy_block *cur_block = resolver->cur_block;
    add_decl_ref(cur_block, &ref);
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
