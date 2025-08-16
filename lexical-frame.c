#include <assert.h>

#include "str.h"
#include "lexical-frame.h"
#include "str-list.h"
#include "map-size-t.h"
#include "stmt-list.h"
#include "resolved-name.h"
#include "errors.h"
#include "var-decl-list.h"
#include "block.h"
#include "function.h"
#include "log.h"

void lexical_frame_assert_valid(const lexical_frame *entry)
{
    switch (entry->type) {
    case LEXICAL_FRAME_BLOCK:
        const lexical_frame_block *block_frame = &entry->block_frame;
        block_assert_valid(block_frame->block);
        break;
    case LEXICAL_FRAME_FUNCTION:
        const lexical_frame_function *func_frame = &entry->function_frame;
        func_assert_valid(func_frame->function);
        break;
    default:
        assert(0);
        break;
    }
    map_size_t_assert_valid(&entry->variables);
}

void lexical_frame_dump(const lexical_frame *entry)
{
    switch (entry->type) {
    case LEXICAL_FRAME_BLOCK:
        const lexical_frame_block *block_frame = &entry->block_frame;
        /* block_dump(f, block_frame->block); */
        log_printf("Block %p\n", block_frame->block);
        break;
    case LEXICAL_FRAME_FUNCTION:
        const lexical_frame_function *func_frame = &entry->function_frame;
        toy_function *func = func_frame->function;
        log_printf("Function call %s(", func->name);
        /* func_dump(f, func); */
        map_size_t_dump(&func_frame->arguments);
        log_printf(")\n");
        break;
    default:
        assert(0);
        break;
    }
}

typedef struct param_name_buildmap_cb_args_struct {
    map_size_t *arguments;
} param_name_buildmap_cb_args;

static item_callback_result param_name_buildmap_callback(void *cookie, size_t index, const toy_str_list *entry)
{
    param_name_buildmap_cb_args *args = (param_name_buildmap_cb_args *) cookie;
    const toy_str param_name = str_list_payload_const(entry);
    map_size_t_set(args->arguments, param_name, index);
    return CONTINUE_ENUMERATION;
}

void lexical_frame_func_build_argmap(lexical_frame_function *func_frame)
{
    param_name_buildmap_cb_args args = { .arguments = &func_frame->arguments };
    enumeration_result res = str_list_foreach_const(func_frame->function->param_names, param_name_buildmap_callback, &args);
    assert(res == ENUMERATION_COMPLETE);
}

typedef struct var_decl_list_lookup_cb_args_struct {
    toy_str desired_name;
    const toy_var_decl *found_decl;
    size_t found_index;
} var_decl_list_lookup_cb_args;

static item_callback_result var_decl_list_lookup_callback(void *cookie, size_t index, const toy_var_decl_list *item)
{
    var_decl_list_lookup_cb_args *args = (var_decl_list_lookup_cb_args *) cookie;
    const toy_var_decl *decl = var_decl_list_payload_const(item);
    if (toy_str_equal(decl->name, args->desired_name)) {
        if (args->found_decl) {
            duplicate_identifier(decl->name);
        } else {
            args->found_decl = decl;
            return STOP_ENUMERATION;
        }
    }
    return CONTINUE_ENUMERATION;
}

static const toy_var_decl *lookup_name_in_var_decl_list(const toy_var_decl_list *var_decl_list, toy_str name, size_t *found_index)
{
    var_decl_list_lookup_cb_args args = { .desired_name = name, .found_decl = NULL, .found_index = 0 };
    enumeration_result res = var_decl_list_foreach_const(var_decl_list, var_decl_list_lookup_callback, &args);
    assert(
        (res == ENUMERATION_COMPLETE && args.found_decl == NULL && args.found_index == 0)
        ||
        (res == ENUMERATION_INTERRUPTED && args.found_decl != NULL && args.found_index >= 0)
    );
    if (args.found_decl) {
        *found_index = args.found_index;
    }
    return args.found_decl;
}

typedef struct stmt_list_lookup_cb_decls_args_struct {
    toy_str desired_name;
    resolved_name *resolved;
    size_t num_variables;
} stmt_list_lookup_decls_cb_args;

static item_callback_result stmt_list_lookup_decls_callback(void *cookie, size_t index, const toy_stmt_list *entry)
{
    stmt_list_lookup_decls_cb_args *args = (stmt_list_lookup_decls_cb_args *) cookie;
    const toy_stmt *stmt = stmt_list_payload_const(entry);
    switch (stmt->type) {
    case STMT_FUNC_DECL:
        const toy_func_decl_stmt *func_decl_stmt = &stmt->func_decl_stmt;
        if (toy_str_equal(func_decl_stmt->func.name, args->desired_name)) {
            if (is_resolved(args->resolved)) {
                duplicate_identifier(func_decl_stmt->func.name);
            } else {
                args->resolved->type = REF_FUNC_DECL;
                args->resolved->func_decl = func_decl_stmt;
            }
        }
        args->num_variables++;
        break;
    case STMT_VAR_DECL:
        const toy_var_decl_list *var_decl_list = &stmt->var_decl_stmt;
        size_t found_index;
        const toy_var_decl *found_var_decl = lookup_name_in_var_decl_list(var_decl_list, args->desired_name, &found_index);
        if (found_var_decl) {
            if (is_resolved(args->resolved)) {
                duplicate_identifier(found_var_decl->name);
            } else {
                args->resolved->type = REF_VAR_DECL;
                args->resolved->var_decl.var_index = args->num_variables + found_index;
            }
        }
        break;
    default:
        /* Ignore other types of statement */
        break;
    }
    return CONTINUE_ENUMERATION;
}

static void lookup_name_in_decls(const lexical_frame *frame, toy_str name, resolved_name *resolved)
{
    lexical_frame_assert_valid(frame);
    stmt_list_lookup_decls_cb_args args = { .desired_name = name, .resolved = resolved, .num_variables = 0 };
    const toy_stmt_list *stmts = lexical_frame_get_stmts(frame);
    enumeration_result res = stmt_list_foreach_const(stmts, stmt_list_lookup_decls_callback, &args);
    assert(res == ENUMERATION_COMPLETE);
}

static void lookup_name_in_params(const lexical_frame_function *func_frame, size_t stack_depth, toy_str name, resolved_name *resolved)
{
    /* TODO: lexical_frame_func_build_argmap(func_frame); */
    const size_t *arg_index = map_size_t_get_const(&func_frame->arguments, name);
    if (arg_index) {
        resolved->type = REF_FUNC_PARAM;
        func_param_ref *param_ref = &resolved->func_param;
        param_ref->param_index = *arg_index;
        param_ref->frames_up = stack_depth;
    } else {
    }
}

void lexical_frame_lookup(const lexical_frame *frame, size_t stack_depth, toy_str name, resolved_name *resolved)
{
    assert(REF_UNDEFINED == resolved->type);
    lexical_frame_assert_valid(frame);
    lookup_name_in_decls(frame, name, resolved);
    if (is_resolved(resolved)) {
        return;
    }
    if (frame->type == LEXICAL_FRAME_FUNCTION) {
        const lexical_frame_function *func_frame = &frame->function_frame;
        lookup_name_in_params(func_frame, stack_depth, name, resolved);
        if (is_resolved(resolved)) {
            return;
        }
    }
}
