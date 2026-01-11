#include <assert.h>

#include "interp.h"
#include "if-arm-list.h"

static run_stmt_result run_if_stmt_block(toy_interp *interp, const toy_block *block)
{
    run_stmt_result res;
    if (block) {
        interp_push_if(interp, block);
        res = interp_run_current_block(interp);
        interp_pop(interp);
    } else {
        res = EXECUTED_STATEMENT;
    }
    return res;
}

static toy_bool if_arm_condition_truthy(toy_interp *interp, const toy_if_arm *arm)
{
    return interp_condition_truthy(interp, arm->condition);
}

typedef struct if_arm_run_cb_args_struct {
    toy_interp *interp;
    const toy_if_arm *found_arm;
} if_arm_run_cb_args;

static item_callback_result if_arm_run_callback(void *cookie, size_t index, const toy_if_arm_list *item)
{
    if_arm_run_cb_args *args = (if_arm_run_cb_args *) cookie;
    const toy_if_arm *arm = if_arm_list_payload_const(item);
    if (if_arm_condition_truthy(args->interp, arm)) {
        args->found_arm = arm;
        return STOP_ENUMERATION;
    }
    return CONTINUE_ENUMERATION;
}

run_stmt_result if_stmt(toy_interp *interp, const toy_if_stmt *if_stmt)
{
    if_arm_run_cb_args args = { .interp = interp, .found_arm = NULL };
    enumeration_result enum_res = if_arm_list_foreach_const(if_stmt->arms, if_arm_run_callback, &args);
    assert(
        (ENUMERATION_COMPLETE == enum_res && NULL == args.found_arm)
        ||
        (ENUMERATION_INTERRUPTED == enum_res && NULL != args.found_arm)
    );
    run_stmt_result run_result;
    if (args.found_arm) {
        run_result = run_if_stmt_block(interp, args.found_arm->code);
    } else {
        run_result = run_if_stmt_block(interp, if_stmt->elsepart);
    }
    if (run_result == REACHED_BLOCK_END) {
        run_result = EXECUTED_STATEMENT;
    }
    return run_result;
}
