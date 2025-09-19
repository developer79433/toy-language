#include <assert.h>

#include "str.h"
#include "lexical-frame.h"
#include "str-list.h"
#include "symbol-table.h"
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
    symbol_table_assert_valid(&entry->variables);
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
        log_printf("Function %s(", func->name);
        /* func_dump(f, func); */
        symbol_table_dump(&func_frame->arguments);
        log_printf(") {\n");
        break;
    default:
        assert(0);
        break;
    }
    log_printf("  Variables { ");
    symbol_table_dump(&entry->variables);
    log_printf(" }\n}\n");
}

typedef struct param_name_buildmap_cb_args_struct {
    symbol_table *arguments;
} param_name_buildmap_cb_args;

static item_callback_result param_name_buildmap_callback(void *cookie, size_t index, const toy_str_list *entry)
{
    param_name_buildmap_cb_args *args = (param_name_buildmap_cb_args *) cookie;
    const toy_str param_name = str_list_payload_const(entry);
    size_t added_index = symbol_table_set(args->arguments, param_name);
    assert(added_index == index);
    return CONTINUE_ENUMERATION;
}

void lexical_frame_func_build_argmap(lexical_frame_function *func_frame)
{
    param_name_buildmap_cb_args args = { .arguments = &func_frame->arguments };
    enumeration_result res = str_list_foreach_const(func_frame->function->param_names, param_name_buildmap_callback, &args);
    assert(res == ENUMERATION_COMPLETE);
}

static void lookup_name_in_decls(const lexical_frame *frame, size_t stack_depth, toy_str name, resolved_name *resolved)
{
    log_printf("Looking up '%s' in declarations\n", name);
    lexical_frame_assert_valid(frame);
    const size_t *variable_index = symbol_table_get_const(&frame->variables, name);
    if (variable_index) {
        /* FIXME: Could be a function, so could be REF_FUNC_DECL. Or merge the two cases? */
        resolved->type = REF_VAR_DECL;
        resolved->var_decl.var_index = *variable_index;
        resolved->var_decl.frames_up = stack_depth;
    }
}

static void lookup_name_in_params(const lexical_frame_function *func_frame, size_t stack_depth, toy_str name, resolved_name *resolved)
{
    log_printf("Looking up '%s' in parameters\n", name);
    const size_t *arg_index = symbol_table_get_const(&func_frame->arguments, name);
    if (arg_index) {
        resolved->type = REF_FUNC_PARAM;
        func_param_ref *param_ref = &resolved->func_param;
        param_ref->param_index = *arg_index;
        param_ref->frames_up = stack_depth;
    }
}

void lexical_frame_lookup(const lexical_frame *frame, size_t stack_depth, toy_str name, resolved_name *resolved)
{
    assert(REF_UNDEFINED == resolved->type);
    lexical_frame_assert_valid(frame);
    lookup_name_in_decls(frame, stack_depth, name, resolved);
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
