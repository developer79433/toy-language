#ifndef TOY_INTERP_H
#define TOY_INTERP_H 1

#include "str-types.h"
#include "val-types.h"
#include "val-list-types.h"
#include "expr-list-types.h"
#include "function-types.h"
#include "interp-types.h"
#include "stmt-types.h"
#include "interp-frame-types.h"

toy_bool is_control_result(run_stmt_result stmt_result);
toy_interp *alloc_interp(const toy_stmt_list *program);
void free_interp(toy_interp *interp);
interp_frame *interp_cur_frame(toy_interp *interp);
void interp_set_cur_frame(toy_interp *interp, interp_frame *frame);
void expr_eval(toy_interp *interp, toy_val *result, const toy_expr *expr);
run_stmt_result run_stmt(toy_interp *interp, const toy_stmt *stmt);
/* TODO: Delete me */
run_stmt_result run_one_stmt(toy_interp *interp, toy_stmt *stmt);
run_stmt_result run_current_block(toy_interp *interp);
void create_variable_expr(toy_interp *interp, const toy_str name, const toy_expr *expr);
int lookup_identifier(toy_interp *interp, toy_val *result, const toy_str name);
toy_bool condition_truthy(toy_interp *interp, const toy_expr *expr);
run_stmt_result call_func(toy_interp *interp, toy_str func_name, const toy_expr_list *args);
run_stmt_result run_toy_function_expr_list(toy_interp *interp, toy_function *def, const toy_expr_list *args);
run_stmt_result run_toy_function_val(toy_interp *interp, toy_function *def, const toy_val *arg);
run_stmt_result run_toy_function_val_list(toy_interp *interp, toy_function *def, const toy_val_list *args);
toy_val *interp_get_return_value(toy_interp *interp);
void interp_set_return_value(toy_interp *interp, toy_val *val);

#endif /* TOY_INTERP_H */
