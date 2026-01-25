#include <assert.h>

#include "interp.h"
#include "if-arm-list.h"
#include "list-visitor.h"

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

typedef struct if_arm_visitor_struct {
    const_list_visitor list_vis;
    toy_interp *interp;
    const toy_if_arm *found_arm;
} if_arm_visitor;

static item_callback_result if_arm_run_callback(if_arm_visitor *if_arm_vis, size_t index, const toy_if_arm_list *item)
{
    const toy_if_arm *arm = if_arm_list_payload_const(item);
    if (if_arm_condition_truthy(if_arm_vis->interp, arm)) {
        if_arm_vis->found_arm = arm;
        return STOP_ENUMERATION;
    }
    return CONTINUE_ENUMERATION;
}

run_stmt_result if_stmt(toy_interp *interp, const toy_if_stmt *if_stmt)
{
    if_arm_visitor if_arm_vis = {
        .list_vis.visit_entry = (const_list_entry_visit_func) if_arm_run_callback,
        .interp = interp,
        .found_arm = NULL
    };
    enumeration_result enum_res = const_list_visitor_visit_list((const_list_visitor *) &if_arm_vis, (const generic_list *) if_stmt->arms);
    assert(
        (ENUMERATION_COMPLETE == enum_res && NULL == if_arm_vis.found_arm)
        ||
        (ENUMERATION_INTERRUPTED == enum_res && NULL != if_arm_vis.found_arm)
    );
    run_stmt_result run_result;
    if (if_arm_vis.found_arm) {
        run_result = run_if_stmt_block(interp, if_arm_vis.found_arm->code);
    } else {
        run_result = run_if_stmt_block(interp, if_stmt->elsepart);
    }
    if (run_result == REACHED_BLOCK_END) {
        run_result = EXECUTED_STATEMENT;
    }
    return run_result;
}
