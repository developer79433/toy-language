#ifndef TOY_INTERP_H
#define TOY_INTERP_H 1

#include "str-types.h"
#include "val-types.h"
#include "val-list-types.h"
#include "expr-list-types.h"
#include "function-types.h"
#include "interp-types.h"
#include "stmt-types.h"
#include "interp-stack-types.h"

toy_bool interp_is_control_result(run_stmt_result stmt_result);
toy_interp *interp_alloc(const toy_stmt_list *program);
void interp_free(toy_interp *interp);
interp_stack *interp_get_stack(toy_interp *interp);
interp_frame *interp_cur_frame(toy_interp *interp);
void interp_push_if(toy_interp *interp, const toy_block *block);
void interp_push_loop(toy_interp *interp, const toy_block *body);
void interp_pop(toy_interp *interp);
void interp_eval(toy_interp *interp, toy_val *result, toy_expr *expr);
run_stmt_result interp_run_stmt(toy_interp *interp, const toy_stmt *stmt);
/* TODO: Delete me */
run_stmt_result interp_run_one_stmt(toy_interp *interp, toy_stmt *stmt);
run_stmt_result interp_run_current_block(toy_interp *interp);
toy_bool interp_condition_truthy(toy_interp *interp, toy_expr *expr);
run_stmt_result interp_call_func(toy_interp *interp, const toy_function *func, const toy_expr_list *args);
run_stmt_result interp_run_func_expr_list(toy_interp *interp, const toy_function *def, const toy_expr_list *args);
run_stmt_result interp_run_func_single_arg(toy_interp *interp, toy_function *def, const toy_val *arg);
run_stmt_result interp_run_func_val_list(toy_interp *interp, toy_function *def, const toy_val_list *args);
toy_val *interp_get_return_value(toy_interp *interp);
void interp_set_return_value(toy_interp *interp, toy_val *val);
toy_val *interp_get_lvalue(toy_interp *interp, toy_identifier *identifier);
const toy_val *interp_get_rvalue(toy_interp *interp, toy_identifier *identifier);

#endif /* TOY_INTERP_H */
