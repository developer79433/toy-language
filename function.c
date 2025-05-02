#include <assert.h>
#include <string.h>

#include "util.h"
#include "function.h"
#include "str.h"
#include "val.h"
#include "val-list.h"
#include "map.h"
#include "dump.h"
#include "constants.h"
#include "operations.h"
#include "errors.h"

void func_dump(FILE *f, const toy_func_def *def)
{
    fprintf(f, "fun %s(", def->name);
    if (def->param_names == &INFINITE_PARAMS) {
        fputs("*", f);
    } else {
        dump_identifier_list(f, def->param_names);
    }
    fputs(") {\n", f);
    if (def->type == FUNC_PREDEFINED) {
        fprintf(f, "/* Pre-defined function code at %p */\n", def->predef);
    } else if (def->type == FUNC_USER_DECLARED) {
        dump_stmts(f, def->code.stmts);
    } else {
        invalid_function_type(def->type);
    }
    fputs("}\n", f);
}

static void predefined_list_len(toy_interp *interp, toy_val *result, const toy_val_list *args)
{
    assert(args);
    const toy_val *arg = &args->val;
    assert(!args->next);
    if (arg->type != VAL_LIST) {
        invalid_argument_type(VAL_LIST, arg);
    }
    result->type = VAL_NUM;
    assert(arg->type == VAL_LIST);
    result->num = val_list_len(arg->list);
}

static void predefined_map_len(toy_interp *interp, toy_val *result, const toy_val_list *args)
{
    assert(args);
    const toy_val *arg = &args->val;
    assert(!args->next);
    if (arg->type != VAL_MAP) {
        invalid_argument_type(VAL_MAP, arg);
    }
    result->type = VAL_NUM;
    assert(arg->type == VAL_MAP);
    result->num = map_len(arg->map);
}

static void predefined_print(toy_interp *interp, toy_val *result, const toy_val_list *args)
{
    for (; args; args = args->next) {
        if (args->val.type == VAL_STR) {
            print_str(stderr, args->val.str);
        } else {
            val_dump(stderr, &args->val);
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
        val_dump(stderr, val);
        fputc('\n', stderr);
    }
    va_end(argptr);
    fatal_error("Assertion failed: %s", msg);
}

static void predefined_assert_equal(toy_interp *interp, toy_val *result, const toy_val_list *args)
{
    assert(val_list_len(args) == 2);
    const toy_val *arg1 = &args->val;
    const toy_val *arg2 = &args->next->val;
    if (vals_equal(arg1, arg2)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be equal", 2, arg1, arg2);
    }
    *result = null_val;
}

static void predefined_assert_not_equal(toy_interp *interp, toy_val *result, const toy_val_list *args)
{
    assert(val_list_len(args) == 2);
    const toy_val *arg1 = &args->val;
    const toy_val *arg2 = &args->next->val;
    if (vals_nequal(arg1, arg2)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should not be equal", 2, arg1, arg2);
    }
    *result = null_val;
}

static void predefined_assert_gt(toy_interp *interp, toy_val *result, const toy_val_list *args)
{
    assert(val_list_len(args) == 2);
    const toy_val *arg1 = &args->val;
    const toy_val *arg2 = &args->next->val;
    if (val_gt(arg1, arg2)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be greater than", 2, arg1, arg2);
    }
    *result = null_val;
}

static void predefined_assert_gte(toy_interp *interp, toy_val *result, const toy_val_list *args)
{
    assert(val_list_len(args) == 2);
    const toy_val *arg1 = &args->val;
    const toy_val *arg2 = &args->next->val;
    if (val_gte(arg1, arg2)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be greater than or equal", 2, arg1, arg2);
    }
    *result = null_val;
}

static void predefined_assert_lt(toy_interp *interp, toy_val *result, const toy_val_list *args)
{
    assert(val_list_len(args) == 2);
    const toy_val *arg1 = &args->val;
    const toy_val *arg2 = &args->next->val;
    if (val_lt(arg1, arg2)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be less than", 2, arg1, arg2);
    }
    *result = null_val;
}

static void predefined_assert_lte(toy_interp *interp, toy_val *result, const toy_val_list *args)
{
    assert(val_list_len(args) == 2);
    const toy_val *arg1 = &args->val;
    const toy_val *arg2 = &args->next->val;
    if (val_lte(arg1, arg2)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be less than or equal", 2, arg1, arg2);
    }
    *result = null_val;
}

static const toy_val zero = { .type = VAL_NUM, .num = 0 };

static void predefined_assert_zero(toy_interp *interp, toy_val *result, const toy_val_list *args)
{
    assert(val_list_len(args) == 1);
    const toy_val *arg1 = &args->val;
    if (vals_equal(arg1, &zero)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be zero", 1, arg1);
    }
    *result = null_val;
}

static void predefined_assert_not_zero(toy_interp *interp, toy_val *result, const toy_val_list *args)
{
    assert(val_list_len(args) == 1);
    const toy_val *arg1 = &args->val;
    if (vals_nequal(arg1, &zero)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be non-zero", 1, arg1);
    }
    *result = null_val;
}

static void predefined_assert_null(toy_interp *interp, toy_val *result, const toy_val_list *args)
{
    assert(val_list_len(args) == 1);
    const toy_val *arg1 = &args->val;
    if (vals_equal(arg1, &null_val)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be null", 1, arg1);
    }
    *result = null_val;
}

static void predefined_assert_not_null(toy_interp *interp, toy_val *result, const toy_val_list *args)
{
    assert(val_list_len(args) == 1);
    const toy_val *arg1 = &args->val;
    if (vals_nequal(arg1, &null_val)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should not be null", 1, arg1);
    }
    *result = null_val;
}

static void predefined_assert(toy_interp *interp, toy_val *result, const toy_val_list *args)
{
    assert(val_list_len(args) == 1);
    assert(!args->next);
    const toy_val *arg = &args->val;
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

static item_callback_result list_foreach_callback(void *cookie, size_t index, const toy_val_list *item)
{
    val_list_foreach_args *args = (val_list_foreach_args *) cookie;
    toy_val_list func_args = { .val = item->val, .next = NULL };
    toy_val result;
    run_toy_function_val_list(args->interp, &result, args->func, &func_args);
    return CONTINUE_ENUMERATION; /* TODO: early bailout? */
}

static void predefined_list_foreach(toy_interp *interp, toy_val *result, const toy_val_list *args)
{
    assert(val_list_len(args) == 2);
    const toy_val *arg1 = &args->val;
    const toy_val *arg2 = &args->next->val;
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

static item_callback_result map_foreach_callback(void *cookie, const toy_str key, const toy_val *value)
{
    map_foreach_args *args = (map_foreach_args *) cookie;
    const toy_val key_val = { .type = VAL_STR, .str = key };
    const toy_val_list value_arg = { .val = *value, .next = NULL };
    const toy_val_list func_args = { .val = key_val, .next = (toy_val_list *) &value_arg };
    toy_val result;
    run_toy_function_val_list(args->interp, &result, args->func, &func_args);
    return CONTINUE_ENUMERATION;
}

static void predefined_map_foreach(toy_interp *interp, toy_val *result, const toy_val_list *args)
{
    assert(val_list_len(args) == 2);
    const toy_val *arg1 = &args->val;
    const toy_val *arg2 = &args->next->val;
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
static const toy_str_list assert_binary_param_2 = { .str = "val2", .next = NULL };
static const toy_str_list assert_binary_params = { .str = "val1", .next = (toy_str_list *) &assert_binary_param_2 };
static const toy_str_list assert_unary_params = { .str = "val", .next = NULL };
static const toy_str_list list_foreach_param_2 = { .str = "func", .next = NULL };
static const toy_str_list list_foreach_params = { .str = "list", .next = (toy_str_list *) &list_foreach_param_2 };
static const toy_str_list list_len_params = { .str = "list", .next = NULL };
static const toy_str_list map_foreach_param_2 = { .str = "func", .next = NULL };
static const toy_str_list map_foreach_params = { .str = "map", .next = (toy_str_list *) &map_foreach_param_2 };
static const toy_str_list map_len_params = { .str = "map", .next = NULL };

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
    { .name = "list_len",   .type = FUNC_PREDEFINED, .predef = predefined_list_len, .param_names = (toy_str_list *) &list_len_params },
    { .name = "list_foreach", .type = FUNC_PREDEFINED, .predef = predefined_list_foreach, .param_names = (toy_str_list *) &list_foreach_params },
    { .name = "map_foreach", .type = FUNC_PREDEFINED, .predef = predefined_map_foreach, .param_names = (toy_str_list *) &map_foreach_params },
    { .name = "map_len",   .type = FUNC_PREDEFINED, .predef = predefined_map_len, .param_names = (toy_str_list *) &map_len_params },
    { .name = "print", .type = FUNC_PREDEFINED, .predef = predefined_print,    .param_names = (toy_str_list *) &INFINITE_PARAMS }
};

const toy_func_def *func_lookup_predef_name(const toy_str name)
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
    return NULL;
}

const toy_func_def *func_lookup_predef_addr(predefined_func_addr func_addr)
{
    ;
    for (
        const toy_func_def *function = &predefined_functions[0];
        function < &predefined_functions[ELEMENTSOF(predefined_functions)];
        function++)
    {
        if (function->predef == func_addr) {
            return function;
        }
    }
    return NULL;
}

void func_assert_valid(const toy_func_def *func_def)
{
    assert(func_def);
    /* TODO */
}

void func_free(toy_func_def *def)
{
    /* TODO */
}
