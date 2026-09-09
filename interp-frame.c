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
#include "func-closure.h"

static const char *frame_type_names[] = {
    "Block statement",
    "If body",
    "Loop body",
    "Pre-defined function",
    "User-defined function"
};

const char *interp_frame_type_name(frame_type type)
{
    return frame_type_names[type];
}

size_t interp_frame_num_vars(const interp_frame *frame)
{
    return frame->num_variables;
}

static void frame_dump_vars(const interp_frame *frame)
{
    if (frame->num_variables) {
        log_debug(" variables [");
    }
    var_array_dump(frame->variables, frame->num_variables, TOY_FALSE);
    if (frame->num_variables) {
        log_debug("] ");
    }
}

static void frame_dump_args(const func_call_frame *call_frame) {
    if (call_frame->num_arguments) {
        log_debug(" arguments [");
    }
    var_array_dump(call_frame->arguments, call_frame->num_arguments, TOY_FALSE);
    if (call_frame->num_arguments) {
        log_debug("] ");
    }
}

void interp_frame_dump(const interp_frame *frame)
{
    interp_frame_assert_valid(frame);
    switch (frame->type) {
    case FRAME_BLOCK_STMT:
        log_debug("Block_stmt %p ", frame->block_stmt.block);
        frame_dump_vars(frame);
        break;
    case FRAME_IF_BODY:
        log_debug("If_body %p ", frame->block_stmt.block);
        frame_dump_vars(frame);
        break;
    case FRAME_LOOP_BODY:
        log_debug("Loop_body %p ", frame->block_stmt.block);
        frame_dump_vars(frame);
        break;
    case FRAME_PRE_DEF_FUNC:
        const func_call_frame *predef_func_call = &frame->func_call;
        const func_closure *predef_closure = predef_func_call->closure;
        assert(0 == predef_closure->num_closures);
        assert(NULL == predef_closure->closures);
        log_debug("Call to predefined ");
        func_closure_dump(predef_closure, TOY_FALSE);
        frame_dump_args(predef_func_call);
        frame_dump_vars(frame);
        break;
    case FRAME_USER_DEF_FUNC:
        const func_call_frame *user_func_call = &frame->func_call;
        const func_closure *user_closure = user_func_call->closure;
        log_debug("Call to user-defined ");
        func_closure_dump(user_closure, TOY_FALSE);
        frame_dump_args(user_func_call);
        frame_dump_vars(frame);
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
        const func_closure *closure = user_func_inv->closure;
        const toy_function *user_func = closure->func;
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
    func_closure_assert_valid(call_frame->closure);
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
