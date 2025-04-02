#include <assert.h>
#include <string.h>

#include "util.h"
#include "functions.h"
#include "list.h"
#include "dump.h"
#include "constants.h"
#include "operations.h"
#include "errors.h"

static void predefined_list_len(toy_interp *interp, toy_expr *result, toy_list *args)
{
    if (args->next) {
        too_many_arguments(1, args);
    }
    toy_expr *arg1 = args->expr;
    if (arg1->type != EXPR_LIST) {
        invalid_operand(EXPR_LIST, arg1);
    }
    result->type = EXPR_NUM;
    result->num = list_len(arg1->list);
}

static const char *predefined_list_len_arg_names[] = {
    "list"
};

static void predefined_print(toy_interp *interp, toy_expr *result, toy_list *args)
{
    for (toy_list *arg = args; arg; arg = arg->next) {
        toy_expr arg_result;
        eval_expr(interp, &arg_result, arg->expr);
        if (arg_result.type == EXPR_STR) {
            print_str(stderr, arg_result.str);
        } else {
            dump_expr(stderr, &arg_result);
        }
        fputc('\n', stderr);
    }
    *result = null_expr;
}

static const char *predefined_assert_binary_param_names[] = {
    "val1",
    "val2"
};

static const char *predefined_assert_unary_param_names[] = {
    "val"
};

static void toy_assert_fail(const char * msg, size_t num_exprs, ...)
{
    va_list argptr;
    va_start(argptr, num_exprs);
    while (num_exprs--) {
        const toy_expr *expr = va_arg(argptr, const toy_expr *);
        dump_expr(stderr, expr);
        fputc('\n', stderr);
    }
    va_end(argptr);
    fatal_error("Assertion failed: %s", msg);
}

static void predefined_assert_equal(toy_interp *interp, toy_expr *result, toy_list *args)
{
    assert(list_len(args) == 2);
    toy_expr *arg1 = args->expr;
    toy_expr *arg2 = args->next->expr;
    op_equal(interp, result, arg1, arg2);
    assert(EXPR_BOOL == result->type);
    if (result->bool) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be equal", 2, arg1, arg2);
    }
}

static void predefined_assert_not_equal(toy_interp *interp, toy_expr *result, toy_list *args)
{
    assert(list_len(args) == 2);
    toy_expr *arg1 = args->expr;
    toy_expr *arg2 = args->next->expr;
    op_nequal(interp, result, arg1, arg2);
    assert(EXPR_BOOL == result->type);
    if (result->bool) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should not be equal", 2, arg1, arg2);
    }
}

static void predefined_assert_gt(toy_interp *interp, toy_expr *result, toy_list *args)
{
    assert(list_len(args) == 2);
    toy_expr *arg1 = args->expr;
    toy_expr *arg2 = args->next->expr;
    op_gt(interp, result, arg1, arg2);
    assert(EXPR_BOOL == result->type);
    if (result->bool) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be greater than", 2, arg1, arg2);
    }
}

static void predefined_assert_gte(toy_interp *interp, toy_expr *result, toy_list *args)
{
    assert(list_len(args) == 2);
    toy_expr *arg1 = args->expr;
    toy_expr *arg2 = args->next->expr;
    op_gte(interp, result, arg1, arg2);
    assert(EXPR_BOOL == result->type);
    if (result->bool) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be greater than or equal", 2, arg1, arg2);
    }
}

static void predefined_assert_lt(toy_interp *interp, toy_expr *result, toy_list *args)
{
    assert(list_len(args) == 2);
    toy_expr *arg1 = args->expr;
    toy_expr *arg2 = args->next->expr;
    op_lt(interp, result, arg1, arg2);
    assert(EXPR_BOOL == result->type);
    if (result->bool) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be less than", 2, arg1, arg2);
    }
}

static void predefined_assert_lte(toy_interp *interp, toy_expr *result, toy_list *args)
{
    assert(list_len(args) == 2);
    toy_expr *arg1 = args->expr;
    toy_expr *arg2 = args->next->expr;
    op_lte(interp, result, arg1, arg2);
    assert(EXPR_BOOL == result->type);
    if (result->bool) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be less than or equal", 2, arg1, arg2);
    }
}

static void predefined_assert_zero(toy_interp *interp, toy_expr *result, toy_list *args)
{
    assert(list_len(args) == 1);
    toy_expr *arg1 = args->expr;
    toy_expr zero = { .type = EXPR_NUM, .num = 0 };
    op_equal(interp, result, arg1, &zero);
    assert(EXPR_BOOL == result->type);
    if (result->bool) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be zero", 1, arg1);
    }
}

static void predefined_assert_non_zero(toy_interp *interp, toy_expr *result, toy_list *args)
{
    assert(list_len(args) == 1);
    toy_expr *arg1 = args->expr;
    toy_expr zero = { .type = EXPR_NUM, .num = 0 };
    op_nequal(interp, result, arg1, &zero);
    assert(EXPR_BOOL == result->type);
    if (result->bool) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be non-zero", 1, arg1);
    }
}

static void predefined_assert_null(toy_interp *interp, toy_expr *result, toy_list *args)
{
    assert(list_len(args) == 1);
    toy_expr *arg1 = args->expr;
    op_equal(interp, result, arg1, &null_expr);
    assert(EXPR_BOOL == result->type);
    if (result->bool) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be null", 1, arg1);
    }
}

static void predefined_assert_non_null(toy_interp *interp, toy_expr *result, toy_list *args)
{
    assert(list_len(args) == 1);
    toy_expr *arg1 = args->expr;
    op_nequal(interp, result, arg1, &null_expr);
    assert(EXPR_BOOL == result->type);
    if (result->bool) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should not be null", 1, arg1);
    }
}

static void predefined_assert(toy_interp *interp, toy_expr *result, toy_list *args)
{
    assert(list_len(args) == 1);
    toy_expr *arg1 = args->expr;
    toy_expr expr_result;
    eval_expr(interp, &expr_result, arg1);
    toy_bool b = convert_to_bool(&expr_result);
    if (b) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be true", 1, arg1);
    }
}

static predefined_function predefined_functions[] = {
    { .name = "len",   .num_params = 1,               .func = predefined_list_len, .param_names = predefined_list_len_arg_names },
    { .name = "print", .num_params = INFINITE_PARAMS, .func = predefined_print,    .param_names = NULL },
    { .name = "assert_equal", .num_params = 2, .func = predefined_assert_equal, .param_names = predefined_assert_binary_param_names },
    { .name = "assert_not_equal", .num_params = 2, .func = predefined_assert_not_equal, .param_names = predefined_assert_binary_param_names },
    { .name = "assert_gt", .num_params = 2, .func = predefined_assert_gt, .param_names = predefined_assert_binary_param_names },
    { .name = "assert_gte", .num_params = 2, .func = predefined_assert_gte, .param_names = predefined_assert_binary_param_names },
    { .name = "assert_lt", .num_params = 2, .func = predefined_assert_lt, .param_names = predefined_assert_binary_param_names },
    { .name = "assert_lte", .num_params = 2, .func = predefined_assert_lte, .param_names = predefined_assert_binary_param_names },
    { .name = "assert", .num_params = 1, .func = predefined_assert, .param_names = predefined_assert_unary_param_names },
    { .name = "assert_zero", .num_params = 1, .func = predefined_assert_zero, .param_names = predefined_assert_unary_param_names },
    { .name = "assert_non_zero", .num_params = 1, .func = predefined_assert_non_zero, .param_names = predefined_assert_unary_param_names },
    { .name = "assert_null", .num_params = 1, .func = predefined_assert_null, .param_names = predefined_assert_unary_param_names },
    { .name = "assert_non_null", .num_params = 1, .func = predefined_assert_non_null, .param_names = predefined_assert_unary_param_names }
};

const predefined_function *lookup_predefined_function(const toy_str name)
{
    for (
        const predefined_function *function = &predefined_functions[0];
        function < &predefined_functions[ELEMENTSOF(predefined_functions)];
        function++)
    {
        if (0 == strcasecmp(function->name, name)) {
            return function;
        }
    }
    return 0;
}
