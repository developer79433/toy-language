#ifndef TOY_OPERATIONS_H
#define TOY_OPERATIONS_H 1

#include "expr-types.h"
#include "interp.h"

void op_and(toy_interp *interp, toy_val *result, const toy_expr *arg1, const toy_expr *arg2);
void op_comma(toy_interp *interp, toy_val *result, const toy_expr *arg1, const toy_expr *arg2);
void op_div(toy_interp *interp, toy_val *result, const toy_expr *arg1, const toy_expr *arg2);
void op_equal(toy_interp *interp, toy_val *result, const toy_expr *arg1, const toy_expr *arg2);
void op_nequal(toy_interp *interp, toy_val *result, const toy_expr *arg1, const toy_expr *arg2);
void op_gt(toy_interp *interp, toy_val *result, const toy_expr *arg1, const toy_expr *arg2);
void op_gte(toy_interp *interp, toy_val *result, const toy_expr *arg1, const toy_expr *arg2);
void op_in(toy_interp *interp, toy_val *result, const toy_expr *arg1, const toy_expr *arg2);
void op_lt(toy_interp *interp, toy_val *result, const toy_expr *arg1, const toy_expr *arg2);
void op_lte(toy_interp *interp, toy_val *result, const toy_expr *arg1, const toy_expr *arg2);
void op_minus(toy_interp *interp, toy_val *result, const toy_expr *arg1, const toy_expr *arg2);
void op_mul(toy_interp *interp, toy_val *result, const toy_expr *arg1, const toy_expr *arg2);
void op_not(toy_interp *interp, toy_val *result, const toy_expr *arg);
void op_or(toy_interp *interp, toy_val *result, const toy_expr *arg1, const toy_expr *arg2);
void op_plus(toy_interp *interp, toy_val *result, const toy_expr *arg1, const toy_expr *arg2);
void op_modulus(toy_interp *interp, toy_val *result, const toy_expr *arg1, const toy_expr *arg2);
void op_uneg(toy_interp *interp, toy_val *result, const toy_expr *arg);
void op_exponent(toy_interp *interp, toy_val *result, const toy_expr *base, const toy_expr *power);
void op_ternary(toy_interp *interp, toy_val *result, toy_expr *condition, toy_expr *if_true, toy_expr *if_false);
void op_method_call(toy_interp *interp, toy_val *result, const toy_method_call *method_call);
void op_field_ref(toy_interp *interp, toy_val *result, toy_str target_name, toy_str field_name);

#endif /* TOY_OPERATIONS_H */
