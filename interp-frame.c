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
        val_list_dump(predef_func_inv->args);
        log_printf(")");
        break;
    case FRAME_USER_DEF_FUNC:
        const func_call_frame *user_func_inv = &frame->func_call;
        const toy_function *user_func = user_func_inv->func;
        log_printf("User-defined function %s(", user_func->name);
        val_list_dump(user_func_inv->args);
        log_printf(")");
        break;
    default:
        assert(0);
        break;
    }
}

void interp_frame_assert_valid(const interp_frame *frame)
{
}

void interp_frame_free(interp_frame *frame)
{
    free(frame);
}

toy_val *interp_frame_get_var(interp_frame *frame, size_t var_index)
{
    return &frame->variables[var_index];
}
