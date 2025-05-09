#include <assert.h>

#include "interp.h"
#include "interp-frame.h"

static void illegal_instruction_in_for_stmt_at_end(const toy_stmt *stmt)
{
    /* TODO */
    assert(0);
}

static void run_for_stmt_at_end(toy_interp *interp, const toy_for_stmt *for_stmt)
{
    if (for_stmt->at_end) {
        run_stmt_result res = run_one_stmt(interp, for_stmt->at_end);
        if (is_control_result(res)) {
            illegal_instruction_in_for_stmt_at_end(for_stmt->at_end);
        }
    }
}

static void illegal_instruction_in_for_stmt_at_start(const toy_stmt *stmt)
{
    /* TODO */
    assert(0);
}

static void run_for_stmt_at_start(toy_interp *interp, const toy_for_stmt *for_stmt)
{
    if (for_stmt->at_start) {
        run_stmt_result res = run_one_stmt(interp, for_stmt->at_start);
        if (is_control_result(res)) {
            illegal_instruction_in_for_stmt_at_start(for_stmt->at_start);
        }
    }
}

static toy_bool for_stmt_condition_truthy(toy_interp *interp, const toy_for_stmt *for_stmt)
{
    return condition_truthy(interp, for_stmt->condition);
}

run_stmt_result for_stmt(toy_interp *interp, const toy_for_stmt *for_stmt)
{
    run_stmt_result res = EXECUTED_STATEMENT;
    for (
        run_for_stmt_at_start(interp, for_stmt);
        /* TRUE */;
        run_for_stmt_at_end(interp, for_stmt)
    ) {
        push_context_loop_body(interp, &for_stmt->body);
        if (!for_stmt_condition_truthy(interp, for_stmt)) {
            pop_context(interp);
            res = EXECUTED_STATEMENT;
            break;
        }
        res = run_current_block(interp);
        pop_context(interp);
        toy_bool break_loop = TOY_FALSE;
        switch (res) {
        case REACHED_RETURN:
            break_loop = TOY_TRUE;
            break;
        case REACHED_BREAK:
            res = EXECUTED_STATEMENT;
            break_loop = TOY_TRUE;
            break;
        case REACHED_CONTINUE:
            continue;
        case REACHED_BLOCK_END:
        case EXECUTED_STATEMENT:
            break;
        default:
            assert(0);
            break;
        }
        if (break_loop) {
            break;
        }
    }
    return res;
}
