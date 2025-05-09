#include <assert.h>

#include "interp.h"

static toy_bool while_stmt_condition_truthy(toy_interp *interp, const toy_while_stmt *while_stmt)
{
    return condition_truthy(interp, while_stmt->condition);
}

run_stmt_result while_stmt(toy_interp *interp, const toy_while_stmt *while_stmt)
{
    run_stmt_result res;
    for (;;) {
        push_context_loop_body(interp, &while_stmt->body);
        if (!while_stmt_condition_truthy(interp, while_stmt)) {
            res = EXECUTED_STATEMENT;
            pop_context(interp);
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
