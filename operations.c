#include <assert.h>
#include <string.h>
#include <math.h>

#include "operations.h"
#include "interp.h"
#include "errors.h"

void op_and(toy_interp *interp, toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1;

    result->type = EXPR_LITERAL;
    result->val.type = VAL_BOOL;
    eval_expr(interp, &arg_result1, arg1);
    assert(EXPR_LITERAL == arg_result1.type);
    toy_bool bool1 = convert_to_bool(&arg_result1.val);
    if (bool1) {
        toy_expr arg_result2;
        eval_expr(interp, &arg_result2, arg2);
        assert(EXPR_LITERAL == arg_result2.type);
        toy_bool bool2 = convert_to_bool(&arg_result2.val);
        result->val.bool = bool2;
    } else {
        result->val.bool = 0;
    }
}

void op_comma(toy_interp *interp, toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1;
    eval_expr(interp, &arg_result1, arg1);
    assert(EXPR_LITERAL == arg_result1.type);
    eval_expr(interp, result, arg2);
    assert(EXPR_LITERAL == result->type);
}

void op_div(toy_interp *interp, toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1, arg_result2;

    eval_expr(interp, &arg_result1, arg1);
    assert(EXPR_LITERAL == arg_result1.type);
    if (arg_result1.val.type != VAL_NUM) {
        invalid_argument_type(VAL_NUM, &arg_result1.val);
        return;
    }
    eval_expr(interp, &arg_result2, arg2);
    assert(EXPR_LITERAL == arg_result2.type);
    if (arg_result2.val.type != VAL_NUM) {
        invalid_argument_type(VAL_NUM, &arg_result2.val);
        return;
    }
    if (0 == arg_result2.val.num) {
        divide_by_zero();
    }
    result->type = EXPR_LITERAL;
    result->val.type = VAL_NUM;
    result->val.num = arg_result1.val.num / arg_result2.val.num;
}

void op_equal(toy_interp *interp, toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1, arg_result2;

    result->type = EXPR_LITERAL;
    result->val.type = VAL_BOOL;
    eval_expr(interp, &arg_result1, arg1);
    assert(EXPR_LITERAL == arg_result1.type);
    eval_expr(interp, &arg_result2, arg2);
    assert(EXPR_LITERAL == arg_result2.type);
    if (arg_result1.val.type == arg_result2.val.type) {
        switch (arg_result1.val.type) {
        case VAL_BOOL:
            result->val.bool = (arg_result1.val.bool == arg_result2.val.bool);
            break;
        case VAL_FUNC:
            result->val.bool = (&arg_result1.val.func.def == &arg_result2.val.func.def);
            break;
        case VAL_LIST:
            result->val.bool = (arg_result1.val.list == arg_result2.val.list);
            break;
        case VAL_MAP:
            result->val.bool = (arg_result1.val.map == arg_result2.val.map);
            break;
        case VAL_NULL:
            result->val.bool = 1;
            break;
        case VAL_NUM:
            result->val.bool = (arg_result1.val.num == arg_result2.val.num);
            break;
        case VAL_STR:
            result->val.bool = (0 == strcmp(arg_result1.val.str, arg_result2.val.str));
            break;
        default:
            invalid_operands(EXPR_EQUAL, &arg_result1, &arg_result2);
            break;
        }
    } else {
        result->val.bool = TOY_FALSE;
    }
}

void op_nequal(toy_interp *interp, toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    op_equal(interp, result, arg1, arg2);
    assert(EXPR_LITERAL == result->type);
    assert(VAL_BOOL == result->val.type);
    result->val.bool = !result->val.bool;
}

void op_gt(toy_interp *interp, toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1, arg_result2;

    eval_expr(interp, &arg_result1, arg1);
    assert(EXPR_LITERAL == arg_result1.type);
    if (arg_result1.val.type != VAL_NUM) {
        invalid_cast(VAL_NUM, &arg_result1);
        return;
    }
    eval_expr(interp, &arg_result2, arg2);
    assert(EXPR_LITERAL == arg_result2.type);
    if (arg_result2.val.type != VAL_NUM) {
        invalid_cast(VAL_NUM, &arg_result2);
        return;
    }
    result->type = EXPR_LITERAL;
    result->val.type = VAL_BOOL;
    result->val.bool = (arg_result1.val.num > arg_result2.val.num);
}

void op_gte(toy_interp *interp, toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1, arg_result2;

    eval_expr(interp, &arg_result1, arg1);
    assert(EXPR_LITERAL == arg_result1.type);
    if (arg_result1.val.type != VAL_NUM) {
        invalid_cast(VAL_NUM, &arg_result1);
        return;
    }
    eval_expr(interp, &arg_result2, arg2);
    assert(EXPR_LITERAL == arg_result2.type);
    if (arg_result2.val.type != VAL_NUM) {
        invalid_cast(VAL_NUM, &arg_result2);
        return;
    }
    result->type = EXPR_LITERAL;
    result->val.type = VAL_BOOL;
    result->val.bool = (arg_result1.val.num >= arg_result2.val.num);
}

void op_in(toy_interp *interp, toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1, arg_result2;

    eval_expr(interp, &arg_result1, arg1);
    assert(EXPR_LITERAL == arg_result1.type);
    eval_expr(interp, &arg_result2, arg2);
    assert(EXPR_LITERAL == arg_result2.type);
    if (arg_result2.val.type != VAL_LIST) {
        invalid_cast(VAL_LIST, &arg_result2);
        return;
    }
    result->type = EXPR_LITERAL;
    result->val.type = VAL_BOOL;
    for (
        const toy_list *list_item = arg_result2.val.list;
        list_item->next;
        list_item = list_item->next
    ) {
        toy_expr comparison_result;
        op_equal(interp, &comparison_result, &arg_result1, list_item->expr);
        assert(EXPR_LITERAL == comparison_result.type);
        assert(comparison_result.val.type == VAL_BOOL);
        if (comparison_result.val.bool) {
            result->val.bool = 1;
            return;
        }
    }
    result->val.bool = TOY_FALSE;
}

void op_lt(toy_interp *interp, toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1, arg_result2;

    eval_expr(interp, &arg_result1, arg1);
    assert(EXPR_LITERAL == arg_result1.type);
    if (arg_result1.val.type != VAL_NUM) {
        invalid_cast(VAL_NUM, &arg_result1);
        return;
    }
    eval_expr(interp, &arg_result2, arg2);
    assert(EXPR_LITERAL == arg_result2.type);
    if (arg_result2.val.type != VAL_NUM) {
        invalid_cast(VAL_NUM, &arg_result2);
        return;
    }
    result->type = EXPR_LITERAL;
    result->val.type = VAL_BOOL;
    result->val.bool = (arg_result1.val.num < arg_result2.val.num);
}

void op_lte(toy_interp *interp, toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1, arg_result2;

    eval_expr(interp, &arg_result1, arg1);
    assert(EXPR_LITERAL == arg_result1.type);
    if (arg_result1.val.type != VAL_NUM) {
        invalid_cast(VAL_NUM, &arg_result1);
        return;
    }
    eval_expr(interp, &arg_result2, arg2);
    assert(EXPR_LITERAL == arg_result2.type);
    if (arg_result2.val.type != VAL_NUM) {
        invalid_cast(VAL_NUM, &arg_result2);
        return;
    }
    result->type = EXPR_LITERAL;
    result->val.type = VAL_BOOL;
    result->val.bool = (arg_result1.val.num <= arg_result2.val.num);
}

void op_minus(toy_interp *interp, toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1, arg_result2;

    eval_expr(interp, &arg_result1, arg1);
    assert(EXPR_LITERAL == arg_result1.type);
    if (arg_result1.val.type != VAL_NUM) {
        invalid_cast(VAL_NUM, &arg_result1);
        return;
    }
    eval_expr(interp, &arg_result2, arg2);
    assert(EXPR_LITERAL == arg_result2.type);
    if (arg_result2.val.type != VAL_NUM) {
        invalid_cast(VAL_NUM, &arg_result2);
        return;
    }
    result->type = EXPR_LITERAL;
    result->val.type = VAL_NUM;
    result->val.num = arg_result1.val.num - arg_result2.val.num;
}

void op_mul(toy_interp *interp, toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1, arg_result2;

    eval_expr(interp, &arg_result1, arg1);
    assert(EXPR_LITERAL == arg_result1.type);
    if (arg_result1.val.type != VAL_NUM) {
        invalid_cast(VAL_NUM, &arg_result1);
        return;
    }
    eval_expr(interp, &arg_result2, arg2);
    assert(EXPR_LITERAL == arg_result2.type);
    if (arg_result2.val.type != VAL_NUM) {
        invalid_cast(VAL_NUM, &arg_result2);
        return;
    }
    result->type = EXPR_LITERAL;
    result->val.type = VAL_NUM;
    result->val.num = arg_result1.val.num * arg_result2.val.num;
}

void op_not(toy_interp *interp, toy_expr *result, const toy_expr *arg)
{
    toy_expr arg_result;

    eval_expr(interp, &arg_result, arg);
    assert(EXPR_LITERAL == arg_result.type);
    toy_bool arg_bool = convert_to_bool(&arg_result.val);
    result->type = EXPR_LITERAL;
    result->val.type = VAL_BOOL;
    result->val.bool = !arg_bool;
}

void op_or(toy_interp *interp, toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1;

    result->type = EXPR_LITERAL;
    result->val.type = VAL_BOOL;
    eval_expr(interp, &arg_result1, arg1);
    assert(EXPR_LITERAL == arg_result1.type);
    toy_bool bool1 = convert_to_bool(&arg_result1.val);
    if (bool1) {
        result->val.bool = TOY_TRUE;
    } else {
        toy_expr arg_result2;
        eval_expr(interp, &arg_result2, arg2);
        assert(EXPR_LITERAL == arg_result2.type);
        toy_bool bool2 = convert_to_bool(&arg_result2.val);
        result->val.bool = bool2;
    }
}

void op_plus(toy_interp *interp, toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1;

    eval_expr(interp, &arg_result1, arg1);
    assert(EXPR_LITERAL == arg_result1.type);
    if (arg_result1.val.type == VAL_NUM) {
        toy_expr arg_result2;
        eval_expr(interp, &arg_result2, arg2);
        assert(EXPR_LITERAL == arg_result2.type);
        if (arg_result2.val.type == VAL_NUM) {
            result->type = EXPR_LITERAL;
            result->val.type = VAL_NUM;
            result->val.num = arg_result1.val.num + arg_result2.val.num;
        } else {
            invalid_operand(EXPR_PLUS, &arg_result2);
        }
    } else if (arg_result1.val.type == VAL_STR) {
        toy_expr arg_result2;
        eval_expr(interp, &arg_result2, arg2);
        assert(EXPR_LITERAL == arg_result2.type);
        if (arg_result2.val.type == VAL_STR) {
            result->type = EXPR_LITERAL;
            result->val.type = VAL_STR;
            result->val.str = (toy_str) malloc(strlen(arg_result1.val.str) + strlen(arg_result2.val.str) + 1);
            sprintf(result->val.str, "%s%s", arg_result1.val.str, arg_result2.val.str);
        } else {
            invalid_operand(EXPR_PLUS, &arg_result2);
        }
    }
}

void op_modulus(toy_interp *interp, toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1, arg_result2;

    eval_expr(interp, &arg_result1, arg1);
    assert(EXPR_LITERAL == arg_result1.type);
    if (arg_result1.val.type != VAL_NUM) {
        invalid_operand(EXPR_MODULUS, &arg_result1);
        return;
    }
    eval_expr(interp, &arg_result2, arg2);
    assert(EXPR_LITERAL == arg_result2.type);
    if (arg_result2.val.type != VAL_NUM) {
        invalid_operand(EXPR_MODULUS, &arg_result2);
        return;
    }
    result->type = EXPR_LITERAL;
    result->val.type = VAL_NUM;
    result->val.num = (int) arg_result1.val.num % (int) arg_result2.val.num;
}

void op_uneg(toy_interp *interp, toy_expr *result, const toy_expr *arg)
{
    toy_expr arg_result;

    eval_expr(interp, &arg_result, arg);
    assert(EXPR_LITERAL == arg_result.type);
    if (arg_result.val.type != VAL_NUM) {
        invalid_cast(VAL_NUM, &arg_result);
        return;
    }
    result->type = EXPR_LITERAL;
    result->val.type = VAL_NUM;
    result->val.num = -arg_result.val.num;
}

void op_exponent(toy_interp *interp, toy_expr *result, const toy_expr *base, const toy_expr *power)
{
    toy_expr base_result, power_result;

    eval_expr(interp, &base_result, base);
    assert(EXPR_LITERAL == base_result.type);
    if (base_result.val.type != VAL_NUM) {
        invalid_operand(EXPR_EXPONENT, &base_result);
        return;
    }
    eval_expr(interp, &power_result, power);
    assert(EXPR_LITERAL == power_result.type);
    if (power_result.val.type != VAL_NUM) {
        invalid_operand(EXPR_EXPONENT, &power_result);
        return;
    }
    result->type = EXPR_LITERAL;
    result->val.type = VAL_NUM;
    result->val.num = pow(base_result.val.num, power_result.val.num);
}

void op_ternary(toy_interp *interp, toy_expr *result, toy_expr *condition, toy_expr *if_true, toy_expr *if_false)
{
    toy_expr cond_result;
    eval_expr(interp, &cond_result, condition);
    assert(EXPR_LITERAL == cond_result.type);
    if (cond_result.val.type == VAL_BOOL) {
        if (cond_result.val.bool) {
            eval_expr(interp, result, if_true);
        } else {
            eval_expr(interp, result, if_false);
        }
    } else {
        invalid_operand(EXPR_TERNARY, &cond_result);
    }
}

static void lookup_field(toy_expr *result, toy_expr *target, toy_str field_name)
{
    /* TODO */
}

void op_field_ref(toy_interp *interp, toy_expr *result, toy_str target_name, toy_str field_name)
{
    toy_expr target;
    int target_exists = lookup_identifier(interp, &target, target_name);
    if (target_exists) {
        lookup_field(result, &target, field_name);
    } else {
        undeclared_identifier(target_name);
    }
}

void op_method_call(toy_interp *interp, toy_expr *result, toy_str target_name, toy_str method_name, toy_list *args)
{
    toy_expr target;
    int target_exists = lookup_identifier(interp, &target, target_name);
    if (target_exists) {
        toy_expr field;
        lookup_field(&field, &target, method_name);
        assert(EXPR_LITERAL == field.type);
        if (field.val.type == VAL_FUNC) {
            run_toy_function(interp, result, &field.val.func.def.code, field.val.func.def.param_names, args);
        } else {
            invalid_operand(EXPR_FUNC_CALL, &field);
        }
    } else {
        undeclared_identifier(target_name);
    }
}
