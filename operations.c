#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "toy-val.h"
#include "toy-str.h"
#include "operations.h"
#include "interp.h"
#include "errors.h"

void op_and(toy_interp *interp, toy_val *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_val arg_result1;

    result->type = VAL_BOOL;
    eval_expr(interp, &arg_result1, arg1);
    toy_bool bool1 = convert_to_bool(&arg_result1);
    if (bool1) {
        toy_val arg_result2;
        eval_expr(interp, &arg_result2, arg2);
        toy_bool bool2 = convert_to_bool(&arg_result2);
        result->boolean = bool2;
    } else {
        result->boolean = TOY_FALSE;
    }
}

void op_comma(toy_interp *interp, toy_val *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_val arg_result1;
    eval_expr(interp, &arg_result1, arg1);
    eval_expr(interp, result, arg2);
}

void op_div(toy_interp *interp, toy_val *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_val arg_result1, arg_result2;

    eval_expr(interp, &arg_result1, arg1);
    if (arg_result1.type != VAL_NUM) {
        invalid_argument_type(VAL_NUM, &arg_result1);
        return;
    }
    eval_expr(interp, &arg_result2, arg2);
    if (arg_result2.type != VAL_NUM) {
        invalid_argument_type(VAL_NUM, &arg_result2);
        return;
    }
    if (0 == arg_result2.num) {
        divide_by_zero();
    }
    result->type = VAL_NUM;
    result->num = arg_result1.num / arg_result2.num;
}

void op_equal(toy_interp *interp, toy_val *result, const toy_expr *expr1, const toy_expr *expr2)
{
    toy_val val1, val2;

    eval_expr(interp, &val1, expr1);
    eval_expr(interp, &val2, expr2);
    result->type = VAL_BOOL;
    result->boolean = toy_vals_equal(&val1, &val2);
}

void op_nequal(toy_interp *interp, toy_val *result, const toy_expr *expr1, const toy_expr *expr2)
{
    toy_val val1, val2;

    eval_expr(interp, &val1, expr1);
    eval_expr(interp, &val2, expr2);
    result->type = VAL_BOOL;
    result->boolean = toy_vals_nequal(&val1, &val2);
}

typedef void (*binary_op_func)(toy_val *result, const toy_val *val1, const toy_val *val2);

static void do_numeric_binary_op(binary_op_func binop_func, toy_val *result, const toy_val *val1, const toy_val *val2)
{
    if (val1->type != VAL_NUM) {
        invalid_cast(VAL_NUM, val1);
        return;
    }
    if (val2->type != VAL_NUM) {
        invalid_cast(VAL_NUM, val2);
        return;
    }
    binop_func(result, val1, val2);
}

static void do_op_gt(toy_val *result, const toy_val *val1, const toy_val *val2)
{
    result->type = VAL_BOOL;
    result->boolean = toy_val_gt(val1, val2);
}

static void do_op_gte(toy_val *result, const toy_val *val1, const toy_val *val2)
{
    result->type = VAL_BOOL;
    result->boolean = toy_val_gte(val1, val2);
}

void op_gt(toy_interp *interp, toy_val *result, const toy_expr *expr1, const toy_expr *expr2)
{
    toy_val val1, val2;

    eval_expr(interp, &val1, expr1);
    eval_expr(interp, &val2, expr2);
    do_numeric_binary_op(do_op_gt, result, &val1, &val2);
}

void op_gte(toy_interp *interp, toy_val *result, const toy_expr *expr1, const toy_expr *expr2)
{
    toy_val val1, val2;

    eval_expr(interp, &val1, expr1);
    eval_expr(interp, &val2, expr2);
    do_numeric_binary_op(do_op_gte, result, &val1, &val2);
}

void op_in(toy_interp *interp, toy_val *result, const toy_expr *needle_expr, const toy_expr *haystack_expr)
{
    toy_val needle, haystack;

    eval_expr(interp, &needle, needle_expr);
    eval_expr(interp, &haystack, haystack_expr);
    if (haystack.type != VAL_LIST) {
        invalid_operand(EXPR_IN, &haystack);
        return;
    }
    result->type = VAL_BOOL;
    assert(haystack.type == VAL_LIST);
    for (
        const toy_val_list *list_item = haystack.list;
        list_item;
        list_item = list_item->next
    ) {
        if (toy_vals_equal(list_item->val, &needle)) {
            result->boolean = TOY_TRUE;
            return;
        }
    }
    result->boolean = TOY_FALSE;
}

static void do_op_lt(toy_val *result, const toy_val *val1, const toy_val *val2)
{
    assert(val1->type == VAL_NUM);
    assert(val2->type == VAL_NUM);
    result->type = VAL_BOOL;
    result->boolean = (val1->num < val2->num);
}

void op_lt(toy_interp *interp, toy_val *result, const toy_expr *expr1, const toy_expr *expr2)
{
    toy_val val1, val2;

    eval_expr(interp, &val1, expr1);
    eval_expr(interp, &val2, expr2);
    do_numeric_binary_op(do_op_lt, result, &val1, &val2);
}

static void do_op_lte(toy_val *result, const toy_val *val1, const toy_val *val2)
{
    assert(val1->type == VAL_NUM);
    assert(val2->type == VAL_NUM);
    result->type = VAL_BOOL;
    result->boolean = (val1->num <= val2->num);
}

void op_lte(toy_interp *interp, toy_val *result, const toy_expr *expr1, const toy_expr *expr2)
{
    toy_val val1, val2;

    eval_expr(interp, &val1, expr1);
    eval_expr(interp, &val2, expr2);
    do_numeric_binary_op(do_op_lte, result, &val1, &val2);
}

static void do_op_minus(toy_val *result, const toy_val *val1, const toy_val *val2)
{
    assert(val1->type == VAL_NUM);
    assert(val2->type == VAL_NUM);
    result->type = VAL_NUM;
    result->num = (val1->num - val2->num);
}

void op_minus(toy_interp *interp, toy_val *result, const toy_expr *expr1, const toy_expr *expr2)
{
    toy_val val1, val2;

    eval_expr(interp, &val1, expr1);
    eval_expr(interp, &val2, expr2);
    do_numeric_binary_op(do_op_minus, result, &val1, &val2);
}

static void do_op_mul(toy_val *result, const toy_val *val1, const toy_val *val2)
{
    assert(val1->type == VAL_NUM);
    assert(val2->type == VAL_NUM);
    result->type = VAL_NUM;
    result->num = (val1->num * val2->num);
}

void op_mul(toy_interp *interp, toy_val *result, const toy_expr *expr1, const toy_expr *expr2)
{
    toy_val val1, val2;

    eval_expr(interp, &val1, expr1);
    eval_expr(interp, &val2, expr2);
    do_numeric_binary_op(do_op_mul, result, &val1, &val2);
}

void op_not(toy_interp *interp, toy_val *result, const toy_expr *expr)
{
    toy_val val;

    eval_expr(interp, &val, expr);
    toy_bool arg_bool = convert_to_bool(&val);
    result->type = VAL_BOOL;
    result->boolean = !arg_bool;
}

void op_or(toy_interp *interp, toy_val *result, const toy_expr *expr1, const toy_expr *expr2)
{
    toy_val val1;

    result->type = VAL_BOOL;
    eval_expr(interp, &val1, expr1);
    toy_bool bool1 = convert_to_bool(&val1);
    if (bool1) {
        result->boolean = TOY_TRUE;
    } else {
        toy_val val2;
        eval_expr(interp, &val2, expr2);
        toy_bool bool2 = convert_to_bool(&val2);
        result->boolean = bool2;
    }
}

static void do_op_plus(toy_val *result, const toy_val *val1, const toy_val *val2)
{
    assert(val1->type == VAL_NUM);
    assert(val2->type == VAL_NUM);
    result->type = VAL_NUM;
    result->num = (val1->num + val2->num);
}

void op_plus(toy_interp *interp, toy_val *result, const toy_expr *expr1, const toy_expr *expr2)
{
    toy_val val1;

    eval_expr(interp, &val1, expr1);
    if (val1.type == VAL_NUM) {
        toy_val val2;
        eval_expr(interp, &val2, expr2);
        if (val2.type == VAL_NUM) {
            do_numeric_binary_op(do_op_plus, result, &val1, &val2);
        } else {
            invalid_operand(EXPR_PLUS, &val2);
        }
    } else if (val1.type == VAL_STR) {
        toy_val val2;
        eval_expr(interp, &val2, expr2);
        if (val2.type == VAL_STR) {
            result->type = VAL_STR;
            result->str = (toy_str) malloc(strlen(val1.str) + strlen(val2.str) + 1);
            sprintf(result->str, "%s%s", val1.str, val2.str);
        } else {
            invalid_operand(EXPR_PLUS, &val2);
        }
    }
}

static void do_op_modulus(toy_val *result, const toy_val *val1, const toy_val *val2)
{
    assert(val1->type == VAL_NUM);
    assert(val2->type == VAL_NUM);
    result->type = VAL_NUM;
    result->num = ((int) val1->num % (int) val2->num);
}

void op_modulus(toy_interp *interp, toy_val *result, const toy_expr *expr1, const toy_expr *expr2)
{
    toy_val val1, val2;

    eval_expr(interp, &val1, expr1);
    eval_expr(interp, &val2, expr2);
    do_numeric_binary_op(do_op_modulus, result, &val1, &val2);
}

void op_uneg(toy_interp *interp, toy_val *result, const toy_expr *arg)
{
    toy_val arg_result;

    eval_expr(interp, &arg_result, arg);
    if (arg_result.type != VAL_NUM) {
        invalid_cast(VAL_NUM, &arg_result);
        return;
    }
    result->type = VAL_NUM;
    result->num = -arg_result.num;
}

static void do_op_exponent(toy_val *result, const toy_val *val1, const toy_val *val2)
{
    assert(val1->type == VAL_NUM);
    assert(val2->type == VAL_NUM);
    result->type = VAL_NUM;
    result->num = pow(val1->num, val2->num);
}

void op_exponent(toy_interp *interp, toy_val *result, const toy_expr *expr1, const toy_expr *expr2)
{
    toy_val val1, val2;

    eval_expr(interp, &val1, expr1);
    eval_expr(interp, &val2, expr2);
    do_numeric_binary_op(do_op_exponent, result, &val1, &val2);
}

void op_ternary(toy_interp *interp, toy_val *result, toy_expr *condition, toy_expr *if_true, toy_expr *if_false)
{
    toy_val cond_result;
    eval_expr(interp, &cond_result, condition);
    if (cond_result.type == VAL_BOOL) {
        if (cond_result.boolean) {
            eval_expr(interp, result, if_true);
        } else {
            eval_expr(interp, result, if_false);
        }
    } else {
        invalid_operand(EXPR_TERNARY, &cond_result);
    }
}

static void lookup_field(toy_val *result, toy_val *target, toy_str field_name)
{
    /* TODO */
}

void op_field_ref(toy_interp *interp, toy_val *result, toy_str target_name, toy_str field_name)
{
    toy_val target;
    int target_exists = lookup_identifier(interp, &target, target_name);
    if (target_exists) {
        lookup_field(result, &target, field_name);
    } else {
        undeclared_identifier(target_name);
    }
}

void op_method_call(toy_interp *interp, toy_val *result, const toy_method_call *method_call)
{
    toy_val target;
    int target_exists = lookup_identifier(interp, &target, method_call->target);
    if (target_exists) {
        toy_val field;
        lookup_field(&field, &target, method_call->method_name);
        if (field.type == VAL_FUNC) {
            run_toy_function_expr_list(interp, result, field.func, method_call->args);
        } else {
            invalid_operand(EXPR_FUNC_CALL, &field);
        }
    } else {
        undeclared_identifier(method_call->target);
    }
}
