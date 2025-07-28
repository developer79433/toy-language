#include <assert.h>

#include "interp-frame.h"
#include "interp.h"
#include "mymalloc.h"
#include "map-val.h"
#include "function.h"
#include "symbol-table.h"
#include "debug.h"

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
        fprintf(f, "block_stmt %p", frame->block_stmt);
        break;
    case FRAME_IF_BODY:
        fprintf(f, "if_body %p", frame->if_body);
        break;
    case FRAME_LOOP_BODY:
        fprintf(f, "loop_body %p", frame->loop_body);
        break;
    case FRAME_PRE_DEF_FUNC:
        fprintf(f, "Predefined function %s", frame->pre_def_func->name);
        break;
    case FRAME_USER_DEF_FUNC:
        fprintf(f, "User-defined function %s", frame->user_def_func->name);
        break;
    default:
        assert(0);
        break;
    }
    fprintf(stderr, ", symbols=");
    symbol_table_dump(f, &frame->symbols);
}

get_result lookup_identifier_in_frame(interp_frame *frame, toy_val *result, toy_str name)
{
    toy_val *existing_value = symbol_table_get(&frame->symbols, name);
    if (existing_value) {
        *result = *existing_value;
        return GET_FOUND;
    }
    return GET_NOT_FOUND;
}

void interp_frame_assert_valid(const interp_frame *frame)
{
    if (valid_check_depth < VALID_CHECK_RECURSION_DEPTH) {
        valid_check_depth++;
        symbol_table_assert_valid(&frame->symbols);
        valid_check_depth--;
    }
}

void interp_frame_free(interp_frame *frame)
{
    symbol_table_free(&frame->symbols);
    free(frame);
}
