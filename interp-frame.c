#include <assert.h>

#include "interp-frame.h"
#include "interp.h"
#include "mymalloc.h"
#include "map-val.h"
#include "function.h"
#include "symbol-table.h"
#include "debug.h"
#include "val-list.h"
#include "log.h"
#include "val.h"
#include "block.h"
#include "var.h"

static const char *frame_type_names[] = {
    "Loop body",
    "If body",
    "Pre-defined function",
    "User-defined function",
    "Block statement"
};

const char *interp_frame_type_name(frame_type type)
{
    return frame_type_names[type];
}

size_t interp_frame_num_vars(const interp_frame *frame)
{
    return frame->num_variables;
}

size_t interp_frame_num_closures(const interp_frame *frame)
{
    return frame->num_closures;
}

static void frame_dump_var_array(toy_var *var_array, size_t num_vars, toy_str name)
{
    log_debug("%s [", name);
    toy_bool printed_anything = TOY_FALSE;
    for (toy_var *var = &var_array[0]; var < &var_array[num_vars]; var++) {
        if (printed_anything) {
            log_debug(", ");
        }
        toy_val *val = var_get(var);
        val_dump(val, TOY_FALSE);
        printed_anything = TOY_TRUE;
    }
    log_debug("] ");
}

static void frame_dump_vars(const interp_frame *frame)
{
    return frame_dump_var_array(frame->variables, frame->num_variables, "variables");
}

static void frame_dump_closures(const interp_frame *frame)
{
    return frame_dump_var_array(frame->closures, frame->num_closures, "closures");
}

void interp_frame_dump(const interp_frame *frame)
{
    interp_frame_assert_valid(frame);
    switch (frame->type) {
    case FRAME_BLOCK_STMT:
        log_debug("Block_stmt %p ", frame->block_stmt.block);
        frame_dump_vars(frame);
        frame_dump_closures(frame);
        break;
    case FRAME_IF_BODY:
        log_debug("If_body %p ", frame->block_stmt.block);
        frame_dump_vars(frame);
        frame_dump_closures(frame);
        break;
    case FRAME_LOOP_BODY:
        log_debug("Loop_body %p ", frame->block_stmt.block);
        frame_dump_vars(frame);
        frame_dump_closures(frame);
        break;
    case FRAME_PRE_DEF_FUNC:
        const func_call_frame *predef_func_inv = &frame->func_call;
        const toy_function *predef_func = predef_func_inv->func;
        log_debug("Predefined function %s(", predef_func->name);
        var_array_dump(predef_func_inv->arguments, predef_func_inv->num_arguments, TOY_FALSE);
        log_debug(") ");
        frame_dump_vars(frame);
        frame_dump_closures(frame);
        break;
    case FRAME_USER_DEF_FUNC:
        const func_call_frame *user_func_inv = &frame->func_call;
        const toy_function *user_func = user_func_inv->func;
        log_debug("User-defined function %s(", user_func->name);
        var_array_dump(user_func_inv->arguments, user_func_inv->num_arguments, TOY_FALSE);
        log_debug(") ");
        frame_dump_vars(frame);
        frame_dump_closures(frame);
        break;
    default:
        assert(0);
        break;
    }
}

toy_block *interp_frame_get_block(interp_frame *frame)
{
    switch (frame->type) {
    case FRAME_BLOCK_STMT:
    case FRAME_IF_BODY:
    case FRAME_LOOP_BODY:
        return (toy_block *) frame->block_stmt.block;
    case FRAME_PRE_DEF_FUNC:
        assert(0);
        return NULL;
    case FRAME_USER_DEF_FUNC:
        func_call_frame *user_func_inv = &frame->func_call;
        const toy_function *user_func = user_func_inv->func;
        return (toy_block *) &user_func->code;
    default:
        assert(0);
        break;
    }
}

const toy_block *interp_frame_get_block_const(const interp_frame *frame)
{
    return interp_frame_get_block((interp_frame *) frame);
}

static void func_call_frame_assert_valid(const func_call_frame *call_frame)
{
    assert(
        (call_frame->arguments == NULL && call_frame->num_arguments == 0)
        ||
        (call_frame->arguments != NULL && call_frame->num_arguments > 0)
    );
    func_assert_valid(call_frame->func);
    var_array_assert_valid(call_frame->arguments, call_frame->num_arguments);
}

void interp_frame_assert_valid(const interp_frame *frame)
{
    assert(frame);
    assert(
        (0 == frame->num_variables && NULL == frame->variables)
        ||
        (0 != frame->num_variables && NULL != frame->variables)
    );
#if 0
    assert(
        (NULL == frame->variables && NULL == frame->cur_val)
        ||
        /* Allow indexing off the end by one, as we should not be dereferencing it */
        (frame->cur_val >= &frame->variables[0] && frame->cur_val <= &frame->variables[frame->num_variables])
    );
#endif
    var_array_assert_valid(frame->variables, frame->num_variables);
    switch (frame->type) {
    case FRAME_BLOCK_STMT:
        block_assert_valid(frame->block_stmt.block);
        break;
    case FRAME_IF_BODY:
        block_assert_valid(frame->block_stmt.block);
        break;
    case FRAME_LOOP_BODY:
        block_assert_valid(frame->block_stmt.block);
        break;
    case FRAME_PRE_DEF_FUNC:
        const func_call_frame *predef_func_inv = &frame->func_call;
        func_call_frame_assert_valid(predef_func_inv);
        break;
    case FRAME_USER_DEF_FUNC:
        const func_call_frame *user_func_inv = &frame->func_call;
        func_call_frame_assert_valid(user_func_inv);
        break;
    default:
        assert(0);
        break;
    }
}

void interp_frame_free(interp_frame *frame)
{
    interp_frame_assert_valid(frame);
    var_array_free(frame->variables, frame->num_variables);
    switch (frame->type) {
    case FRAME_BLOCK_STMT:
    case FRAME_IF_BODY:
    case FRAME_LOOP_BODY:
        break;
    case FRAME_PRE_DEF_FUNC:
    case FRAME_USER_DEF_FUNC:
        func_call_frame *call_frame = &frame->func_call;
        var_array_free(call_frame->arguments, call_frame->num_arguments);
    default:
        assert(0);
        break;
    }
    free(frame);
}

toy_var *interp_frame_get_var(interp_frame *frame, size_t var_index)
{
    interp_frame_assert_valid(frame);
    assert(var_index < frame->num_variables);
    return &frame->variables[var_index];
}

toy_var *interp_frame_get_func_arg(interp_frame *frame, size_t param_index)
{
    interp_frame_assert_valid(frame);
    assert(FRAME_PRE_DEF_FUNC == frame->type || FRAME_USER_DEF_FUNC == frame->type);
    const func_call_frame *call_frame = &frame->func_call;
    assert(param_index < call_frame->num_arguments);
    return &call_frame->arguments[param_index];
}
