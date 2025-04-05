#include <assert.h>
#include <string.h>

#include "util.h"
#include "functions.h"
#include "toy-str.h"
#include "toy-val.h"
#include "toy-val-list.h"
#include "toy-map.h"
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

static void predefined_list_len(toy_interp *interp, toy_val *result, toy_val_list *args)
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

static void predefined_print(toy_interp *interp, toy_val *result, toy_val_list *args)
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

static void predefined_assert_equal(toy_interp *interp, toy_val *result, toy_val_list *args)
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

static void predefined_assert_not_equal(toy_interp *interp, toy_val *result, toy_val_list *args)
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

static void predefined_assert_gt(toy_interp *interp, toy_val *result, toy_val_list *args)
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

static void predefined_assert_gte(toy_interp *interp, toy_val *result, toy_val_list *args)
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

static void predefined_assert_lt(toy_interp *interp, toy_val *result, toy_val_list *args)
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

static void predefined_assert_lte(toy_interp *interp, toy_val *result, toy_val_list *args)
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

static void predefined_assert_zero(toy_interp *interp, toy_val *result, toy_val_list *args)
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

static void predefined_assert_not_zero(toy_interp *interp, toy_val *result, toy_val_list *args)
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

static void predefined_assert_null(toy_interp *interp, toy_val *result, toy_val_list *args)
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

static void predefined_assert_not_null(toy_interp *interp, toy_val *result, toy_val_list *args)
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

static void predefined_assert(toy_interp *interp, toy_val *result, toy_val_list *args)
{
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

typedef struct val_list_foreach_args_struct {
    toy_interp *interp;
    toy_func_def *func;
} val_list_foreach_args;

static void list_foreach_callback(void *cookie, const toy_val *item)
{
    val_list_foreach_args *args = (val_list_foreach_args *) cookie;
    toy_val_list func_args = { .val = (toy_val *) item, .next = 0 };
    toy_val result;
    run_toy_function_val_list(args->interp, &result, args->func, &func_args);
}

static void predefined_list_foreach(toy_interp *interp, toy_val *result, toy_val_list *args)
{
    assert(val_list_len(args) == 2);
    toy_val *arg1 = args->val;
    toy_val *arg2 = args->next->val;
    if (arg1->type == VAL_LIST) {
        toy_val_list *list = arg1->list;
        if (arg2->type == VAL_FUNC) {
            toy_func_def *func = arg2->func;
            val_list_foreach_args cbargs = { .func = func, .interp = interp };
            val_list_foreach_const(list, list_foreach_callback, &cbargs);
        } else {
            invalid_argument_type(VAL_FUNC, arg2);
        }
    } else {
        invalid_argument_type(VAL_LIST, arg1);
    }
    *result = null_val;
}

typedef struct map_foreach_args_struct {
    toy_interp *interp;
    toy_func_def *func;
} map_foreach_args;

static void map_foreach_callback(void *cookie, const toy_str key, const toy_val *value)
{
    map_foreach_args *args = (map_foreach_args *) cookie;
    const toy_val_list value_arg = { .val = (toy_val *) value, .next = NULL };
    toy_val_list func_args = { .val = (toy_val *) key, .next = (toy_val_list *) &value_arg };
    toy_val result;
    run_toy_function_val_list(args->interp, &result, args->func, &func_args);
}

static void predefined_map_foreach(toy_interp *interp, toy_val *result, toy_val_list *args)
{
    assert(val_list_len(args) == 2);
    toy_val *arg1 = args->val;
    toy_val *arg2 = args->next->val;
    if (arg1->type == VAL_MAP) {
        toy_map *map = arg1->map;
        if (arg2->type == VAL_FUNC) {
            toy_func_def *func = arg2->func;
            map_foreach_args cbargs = { .func = func, .interp = interp };
            map_foreach_const(map, map_foreach_callback, &cbargs);
        } else {
            invalid_argument_type(VAL_FUNC, arg2);
        }
    } else {
        invalid_argument_type(VAL_LIST, arg1);
    }
}

const toy_str_list INFINITE_PARAMS;
static const toy_str_list list_len_params = { .str = "list", .next = NULL };
static const toy_str_list assert_binary_param_2 = { .str = "val2", .next = NULL };
static const toy_str_list assert_binary_params = { .str = "val1", .next = (toy_str_list *) &assert_binary_param_2 };
static const toy_str_list assert_unary_params = { .str = "val", .next = NULL };
static const toy_str_list list_foreach_param_2 = { .str = "func", .next = NULL };
static const toy_str_list list_foreach_params = { .str = "list", .next = (toy_str_list *) &list_foreach_param_2 };
static const toy_str_list map_foreach_param_2 = { .str = "func", .next = NULL };
static const toy_str_list map_foreach_params = { .str = "map", .next = (toy_str_list *) &map_foreach_param_2 };

static const toy_func_def predefined_functions[] = {
    { .name = "assert", .type = FUNC_PREDEFINED, .predef = predefined_assert, .param_names = (toy_str_list *) &assert_unary_params },
    { .name = "assert_equal", .type = FUNC_PREDEFINED, .predef = predefined_assert_equal, .param_names = (toy_str_list *) &assert_binary_params },
    { .name = "assert_gt", .type = FUNC_PREDEFINED, .predef = predefined_assert_gt, .param_names = (toy_str_list *) &assert_binary_params },
    { .name = "assert_gte", .type = FUNC_PREDEFINED, .predef = predefined_assert_gte, .param_names = (toy_str_list *) &assert_binary_params },
    { .name = "assert_lt", .type = FUNC_PREDEFINED, .predef = predefined_assert_lt, .param_names = (toy_str_list *) &assert_binary_params },
    { .name = "assert_lte", .type = FUNC_PREDEFINED, .predef = predefined_assert_lte, .param_names = (toy_str_list *) &assert_binary_params },
    { .name = "assert_not_equal", .type = FUNC_PREDEFINED, .predef = predefined_assert_not_equal, .param_names = (toy_str_list *) &assert_binary_params },
    { .name = "assert_not_null", .type = FUNC_PREDEFINED, .predef = predefined_assert_not_null, .param_names = (toy_str_list *) &assert_unary_params },
    { .name = "assert_not_zero", .type = FUNC_PREDEFINED, .predef = predefined_assert_not_zero, .param_names = (toy_str_list *) &assert_unary_params },
    { .name = "assert_null", .type = FUNC_PREDEFINED, .predef = predefined_assert_null, .param_names = (toy_str_list *) &assert_unary_params },
    { .name = "assert_zero", .type = FUNC_PREDEFINED, .predef = predefined_assert_zero, .param_names = (toy_str_list *) &assert_unary_params },
    { .name = "len",   .type = FUNC_PREDEFINED, .predef = predefined_list_len, .param_names = (toy_str_list *) &list_len_params },
    { .name = "list_foreach", .type = FUNC_PREDEFINED, .predef = predefined_list_foreach, .param_names = (toy_str_list *) &list_foreach_params },
    { .name = "map_foreach", .type = FUNC_PREDEFINED, .predef = predefined_map_foreach, .param_names = (toy_str_list *) &map_foreach_params },
    { .name = "print", .type = FUNC_PREDEFINED, .predef = predefined_print,    .param_names = (toy_str_list *) &INFINITE_PARAMS }
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
