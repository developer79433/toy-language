#include "interp.h"

static run_stmt_result run_if_stmt_block(toy_interp *interp, const toy_block *block)
{
    run_stmt_result res;
    if (block) {
        push_context_if_body(interp, block);
        res = run_current_block(interp);
        pop_context(interp);
    } else {
        res = EXECUTED_STATEMENT;
    }
    return res;
}

static toy_bool if_arm_condition_truthy(toy_interp *interp, const toy_if_arm *arm)
{
    return condition_truthy(interp, arm->condition);
}

run_stmt_result if_stmt(toy_interp *interp, const toy_if_stmt *if_stmt)
{
    const toy_if_arm *found_arm = NULL;
    /* TODO: Use if_arm_list_foreach */
    for (toy_if_arm_list *arm_list = if_stmt->arms; arm_list; arm_list = arm_list->next) {
        if (if_arm_condition_truthy(interp, &arm_list->arm)) {
            found_arm = &arm_list->arm;
            break;
        }
    }
    run_stmt_result res;
    if (found_arm) {
        res = run_if_stmt_block(interp, &found_arm->code);
    } else {
        res = run_if_stmt_block(interp, &if_stmt->elsepart);
    }
    if (res == REACHED_BLOCK_END) {
        res = EXECUTED_STATEMENT;
    }
    return res;
}
