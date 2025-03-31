#include <assert.h>
#include <string.h>
#include <math.h>

#include "operations.h"

void op_and(toy_interp *interp, toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1;

    result->type = EXPR_BOOL;
    eval_expr(interp, &arg_result1, arg1);
    toy_bool bool1 = convert_to_bool(&arg_result1);
    if (bool1) {
        toy_expr arg_result2;
        eval_expr(interp, &arg_result2, arg2);
        toy_bool bool2 = convert_to_bool(&arg_result2);
        result->bool = bool2;
    } else {
        result->bool = 0;
    }
}

void op_comma(toy_interp *interp, toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1;
    eval_expr(interp, &arg_result1, arg1);
    eval_expr(interp, result, arg2);
}

void op_div(toy_interp *interp, toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1, arg_result2;

    eval_expr(interp, &arg_result1, arg1);
    if (arg_result1.type != EXPR_NUM) {
        invalid_cast(EXPR_NUM, &arg_result1);
        return;
    }
    eval_expr(interp, &arg_result2, arg2);
    if (arg_result2.type != EXPR_NUM) {
        invalid_cast(EXPR_NUM, &arg_result2);
        return;
    }
    result->type = EXPR_NUM;
    result->num = arg_result1.num / arg_result2.num;
}

void op_equal(toy_interp *interp, toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1, arg_result2;

    result->type = EXPR_BOOL;
    eval_expr(interp, &arg_result1, arg1);
    eval_expr(interp, &arg_result2, arg2);
    if (arg_result1.type == arg_result2.type) {
        switch (arg_result1.type) {
        case EXPR_BOOL:
            result->bool = (arg_result1.bool == arg_result2.bool);
            break;
        case EXPR_NUM:
            result->bool = (arg_result1.num == arg_result2.num);
            break;
        case EXPR_STR:
            result->bool = (0 == strcmp(arg_result1.str, arg_result2.str));
            break;
        default:
            invalid_operand(EXPR_EQUAL, &arg_result1);
            break;
        }
    } else {
        result->bool = 0;
    }
}

void op_gt(toy_interp *interp, toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1, arg_result2;

    eval_expr(interp, &arg_result1, arg1);
    if (arg_result1.type != EXPR_NUM) {
        invalid_cast(EXPR_NUM, &arg_result1);
        return;
    }
    eval_expr(interp, &arg_result2, arg2);
    if (arg_result2.type != EXPR_NUM) {
        invalid_cast(EXPR_NUM, &arg_result2);
        return;
    }
    result->type = EXPR_BOOL;
    result->bool = (arg_result1.num > arg_result2.num);
}

void op_gte(toy_interp *interp, toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1, arg_result2;

    eval_expr(interp, &arg_result1, arg1);
    if (arg_result1.type != EXPR_NUM) {
        invalid_cast(EXPR_NUM, &arg_result1);
        return;
    }
    eval_expr(interp, &arg_result2, arg2);
    if (arg_result2.type != EXPR_NUM) {
        invalid_cast(EXPR_NUM, &arg_result2);
        return;
    }
    result->type = EXPR_BOOL;
    result->bool = (arg_result1.num >= arg_result2.num);
}

void op_in(toy_interp *interp, toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1, arg_result2;

    eval_expr(interp, &arg_result1, arg1);
    eval_expr(interp, &arg_result2, arg2);
    if (arg_result2.type != EXPR_LIST) {
        invalid_cast(EXPR_LIST, &arg_result2);
        return;
    }
    result->type = EXPR_BOOL;
    for (
        const toy_list *list_item = arg_result2.list;
        list_item->next;
        list_item = list_item->next
    ) {
        toy_expr comparison_result;
        op_equal(interp, &comparison_result, &arg_result1, list_item->expr);
        assert(comparison_result.type == EXPR_BOOL);
        if (comparison_result.bool) {
            result->bool = 1;
            return;
        }
    }
    result->bool = 0;
}

void op_lt(toy_interp *interp, toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1, arg_result2;

    eval_expr(interp, &arg_result1, arg1);
    if (arg_result1.type != EXPR_NUM) {
        invalid_cast(EXPR_NUM, &arg_result1);
        return;
    }
    eval_expr(interp, &arg_result2, arg2);
    if (arg_result2.type != EXPR_NUM) {
        invalid_cast(EXPR_NUM, &arg_result2);
        return;
    }
    result->type = EXPR_BOOL;
    result->bool = (arg_result1.num < arg_result2.num);
}

void op_lte(toy_interp *interp, toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1, arg_result2;

    eval_expr(interp, &arg_result1, arg1);
    if (arg_result1.type != EXPR_NUM) {
        invalid_cast(EXPR_NUM, &arg_result1);
        return;
    }
    eval_expr(interp, &arg_result2, arg2);
    if (arg_result2.type != EXPR_NUM) {
        invalid_cast(EXPR_NUM, &arg_result2);
        return;
    }
    result->type = EXPR_BOOL;
    result->bool = (arg_result1.num <= arg_result2.num);
}

void op_minus(toy_interp *interp, toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1, arg_result2;

    eval_expr(interp, &arg_result1, arg1);
    if (arg_result1.type != EXPR_NUM) {
        invalid_cast(EXPR_NUM, &arg_result1);
        return;
    }
    eval_expr(interp, &arg_result2, arg2);
    if (arg_result2.type != EXPR_NUM) {
        invalid_cast(EXPR_NUM, &arg_result2);
        return;
    }
    result->type = EXPR_NUM;
    result->num = arg_result1.num - arg_result2.num;
}

void op_mul(toy_interp *interp, toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1, arg_result2;

    eval_expr(interp, &arg_result1, arg1);
    if (arg_result1.type != EXPR_NUM) {
        invalid_cast(EXPR_NUM, &arg_result1);
        return;
    }
    eval_expr(interp, &arg_result2, arg2);
    if (arg_result2.type != EXPR_NUM) {
        invalid_cast(EXPR_NUM, &arg_result2);
        return;
    }
    result->type = EXPR_NUM;
    result->num = arg_result1.num * arg_result2.num;
}

void op_not(toy_interp *interp, toy_expr *result, const toy_expr *arg)
{
    toy_expr arg_result;

    eval_expr(interp, &arg_result, arg);
    toy_bool arg_bool = convert_to_bool(&arg_result);
    result->type = EXPR_BOOL;
    result->bool = !arg_bool;
}

void op_or(toy_interp *interp, toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1;

    result->type = EXPR_BOOL;
    eval_expr(interp, &arg_result1, arg1);
    toy_bool bool1 = convert_to_bool(&arg_result1);
    if (bool1) {
        result->bool = 1;
    } else {
        toy_expr arg_result2;
        eval_expr(interp, &arg_result2, arg2);
        toy_bool bool2 = convert_to_bool(&arg_result2);
        result->bool = bool2;
    }
}

void op_plus(toy_interp *interp, toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1;

    eval_expr(interp, &arg_result1, arg1);
    if (arg_result1.type == EXPR_NUM) {
        toy_expr arg_result2;
        eval_expr(interp, &arg_result2, arg2);
        if (arg_result2.type == EXPR_NUM) {
            result->type = EXPR_NUM;
            result->num = arg_result1.num + arg_result2.num;
        } else {
            invalid_operand(EXPR_PLUS, &arg_result2);
        }
    } else if (arg_result1.type == EXPR_STR) {
        toy_expr arg_result2;
        eval_expr(interp, &arg_result2, arg2);
        if (arg_result2.type == EXPR_STR) {
            result->type = EXPR_STR;
            result->str = (toy_str) malloc(strlen(arg_result1.str) + strlen(arg_result2.str) + 1);
            sprintf(result->str, "%s%s", arg_result1.str, arg_result2.str);
        } else {
            invalid_operand(EXPR_PLUS, &arg_result2);
        }
    }
}

void op_modulus(toy_interp *interp, toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1, arg_result2;

    eval_expr(interp, &arg_result1, arg1);
    if (arg_result1.type != EXPR_NUM) {
        invalid_operand(EXPR_MODULUS, &arg_result1);
        return;
    }
    eval_expr(interp, &arg_result2, arg2);
    if (arg_result2.type != EXPR_NUM) {
        invalid_operand(EXPR_MODULUS, &arg_result2);
        return;
    }
    result->type = EXPR_NUM;
    result->num = (int) arg_result1.num % (int) arg_result2.num;
}

void op_uneg(toy_interp *interp, toy_expr *result, const toy_expr *arg)
{
    toy_expr arg_result;

    eval_expr(interp, &arg_result, arg);
    if (arg_result.type != EXPR_NUM) {
        invalid_cast(EXPR_NUM, &arg_result);
        return;
    }
    result->type = EXPR_NUM;
    result->num = -arg_result.num;
}

void op_exponent(toy_interp *interp, toy_expr *result, const toy_expr *base, const toy_expr *power)
{
    toy_expr base_result, power_result;

    eval_expr(interp, &base_result, base);
    if (base_result.type != EXPR_NUM) {
        invalid_operand(EXPR_EXPONENT, &base_result);
        return;
    }
    eval_expr(interp, &power_result, power);
    if (power_result.type != EXPR_NUM) {
        invalid_operand(EXPR_EXPONENT, &power_result);
        return;
    }
    result->type = EXPR_NUM;
    result->num = pow(base_result.num, power_result.num);
}

void op_ternary(toy_interp *interp, toy_expr *result, toy_expr *condition, toy_expr *if_true, toy_expr *if_false)
{
    toy_expr cond_result;
    eval_expr(interp, &cond_result, condition);
    if (cond_result.type == EXPR_BOOL) {
        if (cond_result.bool) {
            eval_expr(interp, result, if_true);
        } else {
            eval_expr(interp, result, if_false);
        }
    } else {
        invalid_operand(EXPR_TERNARY, &cond_result);
    }
}

void op_method_call(toy_interp *interp, toy_str target, toy_str func_name, toy_list *args)
{
    /* TODO */
}
