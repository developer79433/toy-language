#include <assert.h>
#include <string.h>

#include "util.h"
#include "functions.h"
#include "toy-str.h"
#include "toy-val.h"
#include "toy-val-list.h"
#include "dump.h"
#include "constants.h"
#include "operations.h"
#include "errors.h"

void dump_function(FILE *f, const toy_func_def *def)
{
    fprintf(f, "fun %s(", def->name);
    dump_identifier_list(f, def->param_names);
    fputs(") {\n", f);
    dump_stmts(f, def->code.stmts);
    fputs("}\n", f);
}

static void predefined_list_len(toy_val *result, toy_val_list *args)
{
    assert(args);
    assert(args->val);
    toy_val *arg = args->val;
    assert(!args->next);
    if (arg->type != VAL_LIST) {
        invalid_argument_type(VAL_LIST, arg);
    }
    result->type = VAL_NUM;
    assert(arg->type == VAL_LIST);
    result->num = val_list_len(arg->list);
}

/* TODO: predefined_map_len */

static void predefined_print(toy_val *result, toy_val_list *args)
{
    for (; args; args = args->next) {
        if (args->val->type == VAL_STR) {
            print_str(stderr, args->val->str);
        } else {
            dump_val(stderr, args->val);
        }
        fputc('\n', stderr);
    }
    *result = null_val;
}

static void toy_assert_fail(const char * msg, size_t num_vals, ...)
{
    va_list argptr;
    va_start(argptr, num_vals);
    while (num_vals--) {
        const toy_val *val = va_arg(argptr, const toy_val *);
        dump_val(stderr, val);
        fputc('\n', stderr);
    }
    va_end(argptr);
    fatal_error("Assertion failed: %s", msg);
}

static void predefined_assert_equal(toy_val *result, toy_val_list *args)
{
    assert(val_list_len(args) == 2);
    toy_val *arg1 = args->val;
    toy_val *arg2 = args->next->val;
    if (toy_vals_equal(arg1, arg2)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be equal", 2, arg1, arg2);
    }
    *result = null_val;
}

static void predefined_assert_not_equal(toy_val *result, toy_val_list *args)
{
    assert(val_list_len(args) == 2);
    toy_val *arg1 = args->val;
    toy_val *arg2 = args->next->val;
    if (toy_vals_nequal(arg1, arg2)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should not be equal", 2, arg1, arg2);
    }
    *result = null_val;
}

static void predefined_assert_gt(toy_val *result, toy_val_list *args)
{
    assert(val_list_len(args) == 2);
    toy_val *arg1 = args->val;
    toy_val *arg2 = args->next->val;
    if (toy_val_gt(arg1, arg2)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be greater than", 2, arg1, arg2);
    }
    *result = null_val;
}

static void predefined_assert_gte(toy_val *result, toy_val_list *args)
{
    assert(val_list_len(args) == 2);
    toy_val *arg1 = args->val;
    toy_val *arg2 = args->next->val;
    if (toy_val_gte(arg1, arg2)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be greater than or equal", 2, arg1, arg2);
    }
    *result = null_val;
}

static void predefined_assert_lt(toy_val *result, toy_val_list *args)
{
    assert(val_list_len(args) == 2);
    toy_val *arg1 = args->val;
    toy_val *arg2 = args->next->val;
    if (toy_val_lt(arg1, arg2)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be less than", 2, arg1, arg2);
    }
    *result = null_val;
}

static void predefined_assert_lte(toy_val *result, toy_val_list *args)
{
    assert(val_list_len(args) == 2);
    toy_val *arg1 = args->val;
    toy_val *arg2 = args->next->val;
    if (toy_val_lte(arg1, arg2)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be less than or equal", 2, arg1, arg2);
    }
    *result = null_val;
}

static const toy_val zero = { .type = VAL_NUM, .num = 0 };

static void predefined_assert_zero(toy_val *result, toy_val_list *args)
{
    assert(val_list_len(args) == 1);
    toy_val *arg1 = args->val;
    if (toy_vals_equal(arg1, &zero)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be zero", 1, arg1);
    }
    *result = null_val;
}

static void predefined_assert_non_zero(toy_val *result, toy_val_list *args)
{
    assert(val_list_len(args) == 1);
    toy_val *arg1 = args->val;
    if (toy_vals_nequal(arg1, &zero)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be non-zero", 1, arg1);
    }
    *result = null_val;
}

static void predefined_assert_null(toy_val *result, toy_val_list *args)
{
    assert(val_list_len(args) == 1);
    toy_val *arg1 = args->val;
    if (toy_vals_equal(arg1, &null_val)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be null", 1, arg1);
    }
    *result = null_val;
}

static void predefined_assert_non_null(toy_val *result, toy_val_list *args)
{
    assert(val_list_len(args) == 1);
    toy_val *arg1 = args->val;
    if (toy_vals_nequal(arg1, &null_val)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should not be null", 1, arg1);
    }
    *result = null_val;
}

static void predefined_assert(toy_val *result, toy_val_list *args)
{
    dump_val_list(stderr, args);
    assert(val_list_len(args) == 1);
    assert(!args->next);
    toy_val *arg = args->val;
    toy_bool b = convert_to_bool(arg);
    if (b) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be true", 1, arg);
    }
    *result = null_val;
}

const toy_str_list INFINITE_PARAMS;
static const toy_str_list list_len_params = { .str = "list", .next = NULL };
static const toy_str_list assert_binary_param_2 = { .str = "val2", .next = NULL };
static const toy_str_list assert_binary_params = { .str = "val1", .next = (toy_str_list *) &assert_binary_param_2 };
static const toy_str_list assert_unary_params = { .str = "val", .next = NULL };

static const toy_func_def predefined_functions[] = {
    { .name = "len",   .type = FUNC_PREDEFINED, .predef = predefined_list_len, .param_names = (toy_str_list *) &list_len_params },
    { .name = "print", .type = FUNC_PREDEFINED, .predef = predefined_print,    .param_names = (toy_str_list *) &INFINITE_PARAMS },
    { .name = "assert_equal", .type = FUNC_PREDEFINED, .predef = predefined_assert_equal, .param_names = (toy_str_list *) &assert_binary_params },
    { .name = "assert_not_equal", .type = FUNC_PREDEFINED, .predef = predefined_assert_not_equal, .param_names = (toy_str_list *) &assert_binary_params },
    { .name = "assert_gt", .type = FUNC_PREDEFINED, .predef = predefined_assert_gt, .param_names = (toy_str_list *) &assert_binary_params },
    { .name = "assert_gte", .type = FUNC_PREDEFINED, .predef = predefined_assert_gte, .param_names = (toy_str_list *) &assert_binary_params },
    { .name = "assert_lt", .type = FUNC_PREDEFINED, .predef = predefined_assert_lt, .param_names = (toy_str_list *) &assert_binary_params },
    { .name = "assert_lte", .type = FUNC_PREDEFINED, .predef = predefined_assert_lte, .param_names = (toy_str_list *) &assert_binary_params },
    { .name = "assert", .type = FUNC_PREDEFINED, .predef = predefined_assert, .param_names = (toy_str_list *) &assert_unary_params },
    { .name = "assert_zero", .type = FUNC_PREDEFINED, .predef = predefined_assert_zero, .param_names = (toy_str_list *) &assert_unary_params },
    { .name = "assert_non_zero", .type = FUNC_PREDEFINED, .predef = predefined_assert_non_zero, .param_names = (toy_str_list *) &assert_unary_params },
    { .name = "assert_null", .type = FUNC_PREDEFINED, .predef = predefined_assert_null, .param_names = (toy_str_list *) &assert_unary_params },
    { .name = "assert_non_null", .type = FUNC_PREDEFINED, .predef = predefined_assert_non_null, .param_names = (toy_str_list *) &assert_unary_params }
};

const toy_func_def *lookup_predefined_function(const toy_str name)
{
    for (
        const toy_func_def *function = &predefined_functions[0];
        function < &predefined_functions[ELEMENTSOF(predefined_functions)];
        function++)
    {
        if (0 == strcasecmp(function->name, name)) {
            return function;
        }
    }
    return 0;
}
