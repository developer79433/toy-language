#include "interp.h"

run_stmt_result return_stmt(toy_interp *interp, const toy_return_stmt *return_stmt)
{
    toy_val val;
    interp_eval_val(interp, &val, return_stmt->expr);
    interp_set_return_value(interp, &val);
    return REACHED_RETURN;
}
