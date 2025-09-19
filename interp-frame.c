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

static void frame_dump_variables(const interp_frame *frame)
{
    log_puts("variables [");
    toy_bool printed_anything = TOY_FALSE;
    for (toy_val *val = &frame->variables[0]; val < &frame->variables[frame->num_variables]; val++) {
        if (printed_anything) {
            log_puts(", ");
        }
        val_dump(val);
        printed_anything = TOY_TRUE;
    }
    log_puts("]");
}

void interp_frame_dump(const interp_frame *frame)
{
    switch (frame->type) {
    case FRAME_BLOCK_STMT:
        log_printf("block_stmt %p", frame->block_stmt.block);
        break;
    case FRAME_IF_BODY:
        log_printf("if_body %p", frame->block_stmt.block);
        break;
    case FRAME_LOOP_BODY:
        log_printf("loop_body %p", frame->block_stmt.block);
        break;
    case FRAME_PRE_DEF_FUNC:
        const func_call_frame *predef_func_inv = &frame->func_call;
        const toy_function *predef_func = predef_func_inv->func;
        log_printf("Predefined function %s(", predef_func->name);
        val_array_dump(predef_func_inv->arguments, predef_func_inv->num_arguments);
        log_printf(") ");
        frame_dump_variables(frame);
        break;
    case FRAME_USER_DEF_FUNC:
        const func_call_frame *user_func_inv = &frame->func_call;
        const toy_function *user_func = user_func_inv->func;
        log_printf("User-defined function %s(", user_func->name);
        val_array_dump(user_func_inv->arguments, user_func_inv->num_arguments);
        log_printf(") ");
        frame_dump_variables(frame);
        break;
    default:
        assert(0);
        break;
    }
}

static void func_call_frame_assert_valid(const func_call_frame *call_frame)
{
    assert(
        (call_frame->arguments == NULL && call_frame->num_arguments == 0)
        ||
        (call_frame->arguments != NULL && call_frame->num_arguments > 0)
    );
    func_assert_valid(call_frame->func);
}

void interp_frame_assert_valid(const interp_frame *frame)
{
    assert(frame);
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
    free(frame);
}

toy_val *interp_frame_get_var(interp_frame *frame, size_t var_index)
{
    interp_frame_assert_valid(frame);
    assert(var_index < frame->num_variables);
    return &frame->variables[var_index];
}

toy_val *interp_frame_get_func_param(interp_frame *frame, size_t var_index)
{
    interp_frame_assert_valid(frame);
    assert(FRAME_PRE_DEF_FUNC == frame->type || FRAME_USER_DEF_FUNC == frame->type);
    const func_call_frame *call_frame = &frame->func_call;
    assert(var_index < call_frame->num_arguments);
    return &call_frame->arguments[var_index];
}
