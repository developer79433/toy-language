#include <assert.h>

#include "lexical-stack.h"
#include "buf-stack.h"
#include "log.h"
#include "stmt-list.h"
#include "var-decl-list.h"
#include "str.h"
#include "errors.h"
#include "str-list.h"
#include "function.h"
#include "val.h"
#include "constants.h"

static lexical_stack_entry *lexical_stack_payload(lexical_stack *stack)
{
    return &stack->entry;
}

static const lexical_stack_entry *lexical_stack_payload_const(const lexical_stack *stack)
{
    return &stack->entry;
}

typedef item_callback_result(*lexical_stack_item_callback)(void *cookie, size_t index, lexical_stack *item);
typedef item_callback_result(*const_lexical_stack_item_callback)(void *cookie, size_t index, const lexical_stack *item);

static enumeration_result lexical_stack_foreach(lexical_stack *stack, lexical_stack_item_callback callback, void *cookie)
{
    return buf_stack_foreach((buf_stack *) stack, (buf_stack_item_callback) callback, cookie);
}

static enumeration_result lexical_stack_foreach_const(const lexical_stack *stack, const_lexical_stack_item_callback callback, void *cookie)
{
    return buf_stack_foreach_const((const buf_stack *) stack, (const_buf_stack_item_callback) callback, cookie);
}

void lexical_stack_init(lexical_stack *stack)
{
    buf_stack_init((buf_stack *) stack);
}

void lexical_stack_dump(const lexical_stack *stack)
{
    log_printf("Lexical stack {\n");
    buf_stack_dump((const buf_stack *) stack);
    log_printf("} End lexical stack\n");
}

lexical_stack *lexical_stack_push(lexical_stack *stack, lexical_stack_entry *entry)
{
    assert(entry->block);
    if (entry->function) {
        func_assert_valid(entry->function);
    }
    log_debug("Stack before push:\n");
    lexical_stack_dump(stack);
    lexical_stack *ret = (lexical_stack *) buf_stack_push((buf_stack *) stack, entry, sizeof(*entry));
    log_debug("Stack after push:\n");
    lexical_stack_dump(ret);
    return ret;
}

lexical_stack *lexical_stack_pop(lexical_stack *stack, lexical_stack_entry **removed_entry)
{
    log_debug("Stack before pop:\n");
    lexical_stack_dump(stack);
    lexical_stack *ret = (lexical_stack *) buf_stack_pop((buf_stack *) stack, (void **) removed_entry);
    log_debug("Stack after pop:\n");
    lexical_stack_dump(ret);
    if (removed_entry) {
        lexical_stack_entry *entry = *removed_entry;
        assert(entry->block);
        if (entry->function) {
            func_assert_valid(entry->function);
        }
    }
    return ret;
}

typedef struct stack_frame_cur_function_cb_args_struct {
    toy_function *found_function;
} stack_frame_cur_function_cb_args;

static item_callback_result stack_frame_cur_function_cb(void *cookie, size_t index, lexical_stack *stack)
{
    stack_frame_cur_function_cb_args *args = (stack_frame_cur_function_cb_args *) cookie;
    lexical_stack_entry *entry = lexical_stack_payload(stack);
    if (entry->function) {
        args->found_function = entry->function;
        return STOP_ENUMERATION;
    }
    return CONTINUE_ENUMERATION;
}

toy_function *cur_function(lexical_stack *stack)
{
    stack_frame_cur_function_cb_args args = { .found_function = NULL };
    enumeration_result res = lexical_stack_foreach(stack, stack_frame_cur_function_cb, &args);
    assert(
        ((ENUMERATION_COMPLETE == res) && !args.found_function)
        ||
        ((ENUMERATION_INTERRUPTED == res) && args.found_function)
    );
    return args.found_function;
}

typedef struct var_decl_list_lookup_cb_args_struct {
    toy_str desired_name;
    toy_var_decl *found_decl;
} var_decl_list_lookup_cb_args;

static item_callback_result var_decl_list_lookup_callback(void *cookie, size_t index, toy_var_decl_list *item)
{
    var_decl_list_lookup_cb_args *args = (var_decl_list_lookup_cb_args *) cookie;
    toy_var_decl *decl = var_decl_list_payload(item);
    if (toy_str_equal(decl->name, args->desired_name)) {
        if (args->found_decl) {
            duplicate_identifier(decl->name);
        } else {
            args->found_decl = decl;
        }
    }
    return CONTINUE_ENUMERATION;
}

static toy_var_decl *lookup_name_in_var_decl_list(toy_var_decl_list *var_decl_list, toy_str name)
{
    var_decl_list_lookup_cb_args args = { .desired_name = name, .found_decl = NULL };
    enumeration_result res = var_decl_list_foreach(var_decl_list, var_decl_list_lookup_callback, &args);
    assert(res == ENUMERATION_COMPLETE);
    return args.found_decl;
}

int is_resolved(const resolved_name *resolved)
{
    switch (resolved->type) {
    case REF_UNDEFINED:
        return 0;
    case REF_FUNC_DECL:
        assert(resolved->func_decl);
        return 1;
    case REF_FUNC_PARAM:
        const func_param_ref *param_ref = &resolved->func_param;
        assert(param_ref->frames_up >= 0);
        assert(param_ref->param_index >= 0);
        return 1;
    case REF_VAR_DECL:
        assert(resolved->var_decl);
        assert(resolved->var_decl->name);
        assert(resolved->var_decl->value);
        return 1;
    case REF_PREDEF_CONST:
        assert(resolved->predef_const);
        predef_const_assert_valid(resolved->predef_const);
        return 1;
    case REF_PREDEF_FUNC:
        assert(resolved->predef_func);
        val_assert_valid(resolved->predef_func);
        return 1;
    default:
        assert(0);
        break;
    }
}

typedef struct stmt_list_lookup_cb_decls_args_struct {
    toy_str desired_name;
    resolved_name *resolved;
} stmt_list_lookup_decls_cb_args;

static item_callback_result stmt_list_lookup_decls_callback(void *cookie, size_t index, toy_stmt_list *entry)
{
    stmt_list_lookup_decls_cb_args *args = (stmt_list_lookup_decls_cb_args *) cookie;
    toy_stmt *stmt = stmt_list_payload(entry);
    switch (stmt->type) {
    case STMT_FUNC_DECL:
        toy_func_decl_stmt *func_decl_stmt = &stmt->func_decl_stmt;
        if (toy_str_equal(func_decl_stmt->func.name, args->desired_name)) {
            if (is_resolved(args->resolved)) {
                duplicate_identifier(func_decl_stmt->func.name);
            } else {
                args->resolved->type = REF_FUNC_DECL;
                args->resolved->func_decl = func_decl_stmt;
            }
        }
        break;
    case STMT_VAR_DECL:
        toy_var_decl_list *var_decl_list = &stmt->var_decl_stmt;
        toy_var_decl *found_var_decl = lookup_name_in_var_decl_list(var_decl_list, args->desired_name);
        if (found_var_decl) {
            if (is_resolved(args->resolved)) {
                duplicate_identifier(found_var_decl->name);
            } else {
                args->resolved->type = REF_VAR_DECL;
                args->resolved->var_decl = found_var_decl;
            }
        }
        break;
    default:
        /* Ignore other types of statement */
        break;
    }
    return CONTINUE_ENUMERATION;
}

static void lookup_name_in_decls(const lexical_stack_entry *frame, toy_str name, resolved_name *resolved)
{
    assert(frame);
    assert(frame->block);
    stmt_list_lookup_decls_cb_args args = { .desired_name = name, .resolved = resolved };
    enumeration_result res = stmt_list_foreach(frame->block->stmts, stmt_list_lookup_decls_callback, &args);
    assert(res == ENUMERATION_COMPLETE);
}

typedef struct param_name_resolve_cb_args_struct {
    toy_str desired_name;
    resolved_name *resolved;
    size_t stack_depth;
} param_name_resolve_cb_args;

/* FIXME: This does a linear search through the list for each named parameter, which is inefficient */

static item_callback_result param_name_resolve_callback(void *cookie, size_t index, const toy_str_list *entry)
{
    param_name_resolve_cb_args *args = (param_name_resolve_cb_args *) cookie;
    const toy_str str = str_list_payload_const(entry);
    if (toy_str_equal(str, args->desired_name)) {
        if (is_resolved(args->resolved)) {
            duplicate_identifier(str);
        } else {
            resolved_name *resolved = args->resolved;
            resolved->type = REF_FUNC_PARAM;
            func_param_ref *param_ref = &resolved->func_param;
            param_ref->param_index = index;
            param_ref->frames_up = args->stack_depth;
        }
    }
    return CONTINUE_ENUMERATION;
}

static void lookup_name_in_params(const toy_function *function, size_t stack_depth, toy_str name, resolved_name *resolved)
{
    param_name_resolve_cb_args args = { .desired_name = name, .resolved = resolved, .stack_depth = stack_depth };
    enumeration_result res = str_list_foreach_const(function->param_names, param_name_resolve_callback, &args);
    assert(res == ENUMERATION_COMPLETE);
}

static void lookup_name_in_frame(const lexical_stack_entry *frame, size_t stack_depth, toy_str name, resolved_name *resolved)
{
    assert(REF_UNDEFINED == resolved->type);
    assert(frame);
    assert(frame->block);
    if (frame->function) {
        func_assert_valid(frame->function);
    }
    lookup_name_in_decls(frame, name, resolved);
    if (is_resolved(resolved)) {
        return;
    }
    if (frame->function) {
        lookup_name_in_params(frame->function, stack_depth, name, resolved);
        if (is_resolved(resolved)) {
            return;
        }
    }
}

typedef struct stack_frame_resolve_cb_args_struct {
    toy_str wanted_name;
    resolved_name *resolved;
} stack_frame_resolve_cb_args;

static item_callback_result stack_frame_resolve_cb(void *cookie, size_t index, const lexical_stack *stack)
{
    stack_frame_resolve_cb_args *args = (stack_frame_resolve_cb_args *) cookie;
    const lexical_stack_entry *entry = lexical_stack_payload_const(stack);
    assert(entry);
    assert(entry->block);
    if (entry->function) {
        func_assert_valid(entry->function);
    }
    lookup_name_in_frame(entry, index, args->wanted_name, args->resolved);
    if (is_resolved(args->resolved)) {
        /* Stop at first (innermost) lexical scope that contains a definition */
        return STOP_ENUMERATION;
    }
    return CONTINUE_ENUMERATION;
}

void lexical_stack_resolve(const lexical_stack *stack, toy_str name, resolved_name *resolved)
{
    assert(stack);
    lexical_stack_dump(stack);
    stack_frame_resolve_cb_args args = { .wanted_name = name, .resolved = resolved };
    enumeration_result res = lexical_stack_foreach_const(stack, stack_frame_resolve_cb, &args);
    assert(
        ((ENUMERATION_COMPLETE == res) && !is_resolved(args.resolved))
        ||
        ((ENUMERATION_INTERRUPTED == res) && is_resolved(args.resolved))
    );
}
