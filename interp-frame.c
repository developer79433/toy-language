#include <assert.h>

#include "interp-frame.h"
#include "interp.h"
#include "mymalloc.h"
#include "map-val.h"
#include "function.h"
#include "symbol-table.h"
#include "debug.h"
#include "val-list.h"

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

void interp_frame_dump(FILE *f, const interp_frame *frame)
{
    switch (frame->type) {
    case FRAME_BLOCK_STMT:
        fprintf(f, "block_stmt %p", frame->block_stmt.block);
        break;
    case FRAME_IF_BODY:
        fprintf(f, "if_body %p", frame->block_stmt.block);
        break;
    case FRAME_LOOP_BODY:
        fprintf(f, "loop_body %p", frame->block_stmt.block);
        break;
    case FRAME_PRE_DEF_FUNC:
        const func_call_frame *predef_func_inv = &frame->func_call;
        const toy_function *predef_func = predef_func_inv->func;
        fprintf(f, "Predefined function %s(", predef_func->name);
        val_list_dump(f, predef_func_inv->args);
        fprintf(f, ")");
        break;
    case FRAME_USER_DEF_FUNC:
        const func_call_frame *user_func_inv = &frame->func_call;
        const toy_function *user_func = user_func_inv->func;
        fprintf(f, "User-defined function %s(", user_func->name);
        val_list_dump(f, user_func_inv->args);
        fprintf(f, ")");
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
