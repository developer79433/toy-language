#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "interp.h"
#include "map.h"

size_t list_len(const toy_list *list)
{
    size_t size;
    for (size = 1; list->next; list = list->next) { size++; }
    return size;
}

size_t map_len(const toy_map *map)
{
    return 0; /* TODO */
}

toy_bool convert_to_bool(const toy_expr *expr)
{
    switch (expr->type) {
    case EXPR_ASSIGN:
        /* TODO */
        break;
    case EXPR_BOOL:
        return expr->bool;
    case EXPR_NUM:
        return expr->num != 0;
    case EXPR_LIST:
        return list_len(expr->list) != 0;
    case EXPR_MAP:
        return map_len(expr->map) != 0;
    case EXPR_STR:
        return strlen(expr->str) != 0;
    case EXPR_IDENTIFIER:
    case EXPR_AND:
    case EXPR_DIV:
    case EXPR_EQUAL:
    case EXPR_FUNC_CALL:
    case EXPR_FUNC_DECL:
    case EXPR_GT:
    case EXPR_GTE:
    case EXPR_LT:
    case EXPR_LTE:
    case EXPR_IN:
    case EXPR_MINUS:
    case EXPR_MUL:
    case EXPR_NEQUAL:
    case EXPR_NOT:
    case EXPR_OR:
    case EXPR_PLUS:
    case EXPR_UNEG:
        invalid_cast(EXPR_BOOL, expr);
        return 0;
    default:
        invalid_expr_type(expr->type);
        break;
    }
    return 0;
}

toy_bool convert_to_num(const toy_expr *expr)
{
    switch (expr->type) {
    case EXPR_ASSIGN:
        /* TODO */
        break;
    case EXPR_BOOL:
        return expr->bool ? 1 : 0;
    case EXPR_NUM:
        return expr->num;
    case EXPR_LIST:
    case EXPR_MAP:
    case EXPR_STR:
    case EXPR_IDENTIFIER:
    case EXPR_AND:
    case EXPR_DIV:
    case EXPR_EQUAL:
    case EXPR_FUNC_CALL:
    case EXPR_FUNC_DECL:
    case EXPR_GT:
    case EXPR_GTE:
    case EXPR_LT:
    case EXPR_LTE:
    case EXPR_IN:
    case EXPR_MINUS:
    case EXPR_MUL:
    case EXPR_NEQUAL:
    case EXPR_NOT:
    case EXPR_OR:
    case EXPR_PLUS:
    case EXPR_UNEG:
        invalid_cast(EXPR_BOOL, expr);
        return 0;
    default:
        invalid_expr_type(expr->type);
        break;
    }
    return 0;
}

static void op_and(toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1;

    result->type = EXPR_BOOL;
    eval_expr(&arg_result1, arg1);
    toy_bool bool1 = convert_to_bool(&arg_result1);
    if (bool1) {
        toy_expr arg_result2;
        eval_expr(&arg_result2, arg2);
        toy_bool bool2 = convert_to_bool(&arg_result2);
        result->bool = bool2;
    } else {
        result->bool = 0;
    }
}

static void op_comma(toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1;
    eval_expr(&arg_result1, arg1);
    eval_expr(result, arg2);
}

static void op_div(toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1, arg_result2;

    eval_expr(&arg_result1, arg1);
    if (arg_result1.type != EXPR_NUM) {
        invalid_cast(EXPR_NUM, &arg_result1);
        return;
    }
    eval_expr(&arg_result2, arg2);
    if (arg_result2.type != EXPR_NUM) {
        invalid_cast(EXPR_NUM, &arg_result2);
        return;
    }
    result->type = EXPR_NUM;
    result->num = arg_result1.num / arg_result2.num;
}

static void op_equal(toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1, arg_result2;

    result->type = EXPR_BOOL;
    eval_expr(&arg_result1, arg1);
    eval_expr(&arg_result2, arg2);
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

static void op_gt(toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1, arg_result2;

    eval_expr(&arg_result1, arg1);
    if (arg_result1.type != EXPR_NUM) {
        invalid_cast(EXPR_NUM, &arg_result1);
        return;
    }
    eval_expr(&arg_result2, arg2);
    if (arg_result2.type != EXPR_NUM) {
        invalid_cast(EXPR_NUM, &arg_result2);
        return;
    }
    result->type = EXPR_BOOL;
    result->bool = (arg_result1.num > arg_result2.num);
}

static void op_gte(toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1, arg_result2;

    eval_expr(&arg_result1, arg1);
    if (arg_result1.type != EXPR_NUM) {
        invalid_cast(EXPR_NUM, &arg_result1);
        return;
    }
    eval_expr(&arg_result2, arg2);
    if (arg_result2.type != EXPR_NUM) {
        invalid_cast(EXPR_NUM, &arg_result2);
        return;
    }
    result->type = EXPR_BOOL;
    result->bool = (arg_result1.num >= arg_result2.num);
}

static void op_in(toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1, arg_result2;

    eval_expr(&arg_result1, arg1);
    eval_expr(&arg_result2, arg2);
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
        op_equal(&comparison_result, &arg_result1, list_item->expr);
        assert(comparison_result.type == EXPR_BOOL);
        if (comparison_result.bool) {
            result->bool = 1;
            return;
        }
    }
    result->bool = 0;
}

static void op_lt(toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1, arg_result2;

    eval_expr(&arg_result1, arg1);
    if (arg_result1.type != EXPR_NUM) {
        invalid_cast(EXPR_NUM, &arg_result1);
        return;
    }
    eval_expr(&arg_result2, arg2);
    if (arg_result2.type != EXPR_NUM) {
        invalid_cast(EXPR_NUM, &arg_result2);
        return;
    }
    result->type = EXPR_BOOL;
    result->bool = (arg_result1.num < arg_result2.num);
}

static void op_lte(toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1, arg_result2;

    eval_expr(&arg_result1, arg1);
    if (arg_result1.type != EXPR_NUM) {
        invalid_cast(EXPR_NUM, &arg_result1);
        return;
    }
    eval_expr(&arg_result2, arg2);
    if (arg_result2.type != EXPR_NUM) {
        invalid_cast(EXPR_NUM, &arg_result2);
        return;
    }
    result->type = EXPR_BOOL;
    result->bool = (arg_result1.num <= arg_result2.num);
}

static void op_minus(toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1, arg_result2;

    eval_expr(&arg_result1, arg1);
    if (arg_result1.type != EXPR_NUM) {
        invalid_cast(EXPR_NUM, &arg_result1);
        return;
    }
    eval_expr(&arg_result2, arg2);
    if (arg_result2.type != EXPR_NUM) {
        invalid_cast(EXPR_NUM, &arg_result2);
        return;
    }
    result->type = EXPR_NUM;
    result->num = arg_result1.num - arg_result2.num;
}

static void op_mul(toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1, arg_result2;

    eval_expr(&arg_result1, arg1);
    if (arg_result1.type != EXPR_NUM) {
        invalid_cast(EXPR_NUM, &arg_result1);
        return;
    }
    eval_expr(&arg_result2, arg2);
    if (arg_result2.type != EXPR_NUM) {
        invalid_cast(EXPR_NUM, &arg_result2);
        return;
    }
    result->type = EXPR_NUM;
    result->num = arg_result1.num * arg_result2.num;
}

static void op_not(toy_expr *result, const toy_expr *arg)
{
    toy_expr arg_result;

    eval_expr(&arg_result, arg);
    toy_bool arg_bool = convert_to_bool(&arg_result);
    result->type = EXPR_BOOL;
    result->bool = !arg_bool;
}

static void op_or(toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1;

    result->type = EXPR_BOOL;
    eval_expr(&arg_result1, arg1);
    toy_bool bool1 = convert_to_bool(&arg_result1);
    if (bool1) {
        result->bool = 1;
    } else {
        toy_expr arg_result2;
        eval_expr(&arg_result2, arg2);
        toy_bool bool2 = convert_to_bool(&arg_result2);
        result->bool = bool2;
    }
}

static void op_plus(toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1;

    eval_expr(&arg_result1, arg1);
    if (arg_result1.type == EXPR_NUM) {
        toy_expr arg_result2;

        eval_expr(&arg_result2, arg2);
        if (arg_result2.type == EXPR_NUM) {
            result->type = EXPR_NUM;
            result->num = arg_result1.num + arg_result2.num;
        } else {
            invalid_operand(EXPR_PLUS, &arg_result2);
        }
    } else if (arg_result1.type == EXPR_STR) {
        toy_expr arg_result2;
        eval_expr(&arg_result2, arg2);
        result->type = EXPR_STR;
        result->str = "";
        /* TODO: append stringified representation of result2 */
    }
}

static void op_modulus(toy_expr *result, const toy_expr *arg1, const toy_expr *arg2)
{
    toy_expr arg_result1, arg_result2;

    eval_expr(&arg_result1, arg1);
    if (arg_result1.type != EXPR_NUM) {
        invalid_operand(EXPR_MODULUS, &arg_result1);
        return;
    }
    eval_expr(&arg_result2, arg2);
    if (arg_result2.type == EXPR_NUM) {
        invalid_operand(EXPR_MODULUS, &arg_result2);
        return;
    }
    result->type = EXPR_NUM;
    result->num = (int) arg_result1.num % (int) arg_result2.num;
}

static void op_uneg(toy_expr *result, const toy_expr *arg)
{
    toy_expr arg_result;

    eval_expr(&arg_result, arg);
    if (arg_result.type != EXPR_NUM) {
        invalid_cast(EXPR_NUM, &arg_result);
        return;
    }
    result->type = EXPR_NUM;
    result->num = -arg_result.num;
}

static void call_func(toy_expr *result, toy_str func_name, toy_list *args)
{
    /* TODO */
}

void eval_expr(toy_expr *result, const toy_expr *expr)
{
    switch (expr->type) {
    case EXPR_AND:
        op_and(result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_ASSIGN:
        /* TODO */
        break;
    case EXPR_BOOL:
        *result = *expr;
        break;
    case EXPR_COMMA:
        op_comma(result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_DIV:
        op_div(result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_EQUAL:
        op_equal(result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_FUNC_CALL:
        call_func(result, expr->func_call.func_name, expr->func_call.args);
        break;
    case EXPR_FUNC_DECL:
        *result = *expr;
        break;
    case EXPR_GT:
        op_gt(result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_GTE:
        op_gte(result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_IDENTIFIER:
        /* TODO */
        break;
    case EXPR_IN:
        op_in(result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_LIST:
        *result = *expr;
        break;
    case EXPR_LT:
        op_lt(result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_LTE:
        op_lte(result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_MAP:
        *result = *expr;
        break;
    case EXPR_MINUS:
        op_minus(result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_MODULUS:
        op_modulus(result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_MUL:
        op_mul(result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_NEQUAL:
        op_equal(result, expr->binary_op.arg1, expr->binary_op.arg2);
        result->bool = !result->bool;
        break;
    case EXPR_NOT:
        op_not(result, expr->unary_op.arg);
        break;
    case EXPR_NUM:
        *result = *expr;
        break;
    case EXPR_OR:
        op_or(result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_PLUS:
        op_plus(result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_POSTFIX_DECREMENT:
        /* TODO */
        break;
    case EXPR_POSTFIX_INCREMENT:
        /* TODO */
        break;
    case EXPR_PREFIX_DECREMENT:
        /* TODO */
        break;
    case EXPR_PREFIX_INCREMENT:
        /* TODO */
        break;
    case EXPR_STR:
        *result = *expr;
        break;
    case EXPR_TERNARY:
        /* TODO */
        break;
    case EXPR_UNEG:
        op_uneg(result, expr->unary_op.arg);
        break;
    default:
        invalid_expr_type(expr->type);
        break;
    }
}

void single_step(const toy_stmt *stmt)
{
    switch (stmt->type) {
    case STMT_EXPR:
        toy_expr result;
        eval_expr(&result, stmt->expr_stmt.expr);
        break;
    case STMT_FOR:
        if (stmt->for_stmt.at_start) {
            single_step(stmt->for_stmt.at_start);
        }
        for (;;) {
            if (stmt->for_stmt.condition) {
                toy_expr cond_result;
                eval_expr(&cond_result, stmt->for_stmt.condition);
                if (EXPR_BOOL != cond_result.type) {
                    invalid_operand(EXPR_BOOL, &cond_result);
                }
                assert(EXPR_BOOL == cond_result.type);
                if (cond_result.bool) {
                    break;
                }
            }
            toy_run(stmt->for_stmt.body.stmts);
            if (stmt->for_stmt.at_end) {
                single_step(stmt->for_stmt.at_end);
            }
        }
        break;
    case STMT_FUNC_DECL:
        /* TODO */
        break;
    case STMT_IF:
        {
            unsigned int found_one = 0;
            for (toy_if_arm *arm = stmt->if_stmt.arms; arm; arm = arm->next) {
                toy_expr cond_result;
                eval_expr(&cond_result, arm->condition);
                if (EXPR_BOOL != cond_result.type) {
                    invalid_operand(EXPR_BOOL, &cond_result);
                }
                assert(EXPR_BOOL == cond_result.type);
                if (cond_result.bool) {
                    toy_run(arm->code.stmts);
                    found_one = 1;
                    break;
                }
            }
            if (!found_one && stmt->if_stmt.elsepart.stmts) {
                toy_run(stmt->if_stmt.elsepart.stmts);
            }
        }
        break;
    case STMT_NULL:
        break;
    case STMT_VAR_DECL:
        /* TODO */
        break;
    case STMT_WHILE:
        for(;;) {
            toy_expr cond_result;
            eval_expr(&cond_result, stmt->while_stmt.condition);
            if (EXPR_BOOL != cond_result.type) {
                invalid_operand(EXPR_BOOL, &cond_result);
            }
            assert(EXPR_BOOL == cond_result.type);
            if (cond_result.bool) {
                break;
            }
            toy_run(stmt->while_stmt.body.stmts);
        }
        break;
    case STMT_RETURN:
        /* TODO */
        break;
    case STMT_BREAK:
        /* TODO */
        break;
    case STMT_CONTINUE:
        /* TODO */
        break;
    default:
        invalid_stmt_type(stmt->type);
        break;
    }
}

void toy_run(const toy_stmt *stmt)
{
    for (const toy_stmt *s = stmt; s; s = s->next) {
        single_step(s);
    }
}

void test_maps()
{
    toy_map *map1 = alloc_map();
    toy_expr val1;
    val1.type = EXPR_NUM;
    val1.num = 42;
    map_set(map1, "first key", &val1);
    toy_expr *get1 = map_get(map1, "first key");
    assert(get1->type == EXPR_NUM);
    assert(get1->num == 42);
    toy_expr val2;
    val2.type = EXPR_STR;
    val2.str = "second value";
    map_set(map1, "second key", &val2);
    toy_expr *get2 = map_get(map1, "second key");
    assert(get2->type == EXPR_STR);
    assert(0 == strcmp(get2->str, "second value"));
    toy_expr *get3 = map_get(map1, "first key");
    assert(get3->type == EXPR_NUM);
    assert(get3->num == 42);
    dump_map(stderr, map1);
}
