#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "predef-function.h"
#include "util.h"
#include "errors.h"
#include "interp.h"
#include "val-list.h"
#include "map-val.h"
#include "val.h"
#include "str.h"
#include "constants.h"
#include "log.h"

static run_stmt_result predefined_list_len(toy_interp *interp, const toy_val_list *args)
{
    assert(args);
    const toy_val *arg = &args->val;
    assert(!args->next);
    if (arg->type != VAL_LIST) {
        invalid_argument_type(VAL_LIST, arg);
    }
    toy_val *return_val = interp_get_return_value(interp);
    return_val->type = VAL_NUM;
    assert(arg->type == VAL_LIST);
    return_val->num = val_list_len(arg->list);
    return REACHED_RETURN;
}

static run_stmt_result predefined_map_len(toy_interp *interp, const toy_val_list *args)
{
    assert(args);
    const toy_val *arg = &args->val;
    assert(!args->next);
    if (arg->type != VAL_MAP) {
        invalid_argument_type(VAL_MAP, arg);
    }
    toy_val *return_val = interp_get_return_value(interp);
    return_val->type = VAL_NUM;
    assert(arg->type == VAL_MAP);
    return_val->num = map_val_size(arg->map);
    return REACHED_RETURN;
}

static run_stmt_result predefined_print(toy_interp *interp, const toy_val_list *args)
{
    for (; args; args = args->next) {
        if (args->val.type == VAL_STR) {
            print_str(args->val.str);
        } else {
            val_dump(&args->val);
        }
        log_putc('\n');
    }
    return REACHED_BLOCK_END;
}

static void toy_assert_fail(const char * msg, size_t num_vals, ...)
{
    va_list argptr;
    va_start(argptr, num_vals);
    while (num_vals--) {
        const toy_val *val = va_arg(argptr, const toy_val *);
        val_dump(val);
        log_putc('\n');
    }
    va_end(argptr);
    fatal_error("Assertion failed: %s", msg);
}

static run_stmt_result predefined_assert_equal(toy_interp *interp, const toy_val_list *args)
{
    assert(val_list_len(args) == 2);
    const toy_val *arg1 = &args->val;
    const toy_val *arg2 = &args->next->val;
    if (vals_equal(arg1, arg2)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be equal", 2, arg1, arg2);
    }
    return REACHED_BLOCK_END;
}

static run_stmt_result predefined_assert_not_equal(toy_interp *interp, const toy_val_list *args)
{
    assert(val_list_len(args) == 2);
    const toy_val *arg1 = &args->val;
    const toy_val *arg2 = &args->next->val;
    if (vals_nequal(arg1, arg2)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should not be equal", 2, arg1, arg2);
    }
    return REACHED_BLOCK_END;
}

static run_stmt_result predefined_assert_gt(toy_interp *interp, const toy_val_list *args)
{
    assert(val_list_len(args) == 2);
    const toy_val *arg1 = &args->val;
    const toy_val *arg2 = &args->next->val;
    if (val_gt(arg1, arg2)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be greater than", 2, arg1, arg2);
    }
    return REACHED_BLOCK_END;
}

static run_stmt_result predefined_assert_gte(toy_interp *interp, const toy_val_list *args)
{
    assert(val_list_len(args) == 2);
    const toy_val *arg1 = &args->val;
    const toy_val *arg2 = &args->next->val;
    if (val_gte(arg1, arg2)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be greater than or equal", 2, arg1, arg2);
    }
    return REACHED_BLOCK_END;
}

static run_stmt_result predefined_assert_lt(toy_interp *interp, const toy_val_list *args)
{
    assert(val_list_len(args) == 2);
    const toy_val *arg1 = &args->val;
    const toy_val *arg2 = &args->next->val;
    if (val_lt(arg1, arg2)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be less than", 2, arg1, arg2);
    }
    return REACHED_BLOCK_END;
}

static run_stmt_result predefined_assert_lte(toy_interp *interp, const toy_val_list *args)
{
    assert(val_list_len(args) == 2);
    const toy_val *arg1 = &args->val;
    const toy_val *arg2 = &args->next->val;
    if (val_lte(arg1, arg2)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be less than or equal", 2, arg1, arg2);
    }
    return REACHED_BLOCK_END;
}

static const toy_val zero = { .type = VAL_NUM, .num = 0 };

static run_stmt_result predefined_assert_zero(toy_interp *interp, const toy_val_list *args)
{
    assert(val_list_len(args) == 1);
    const toy_val *arg1 = &args->val;
    if (vals_equal(arg1, &zero)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be zero", 1, arg1);
    }
    return REACHED_BLOCK_END;
}

static run_stmt_result predefined_assert_not_zero(toy_interp *interp, const toy_val_list *args)
{
    assert(val_list_len(args) == 1);
    const toy_val *arg1 = &args->val;
    if (vals_nequal(arg1, &zero)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be non-zero", 1, arg1);
    }
    return REACHED_BLOCK_END;
}

static run_stmt_result predefined_assert_null(toy_interp *interp, const toy_val_list *args)
{
    assert(val_list_len(args) == 1);
    const toy_val *arg1 = &args->val;
    if (vals_equal(arg1, &null_val)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be null", 1, arg1);
    }
    return REACHED_BLOCK_END;
}

static run_stmt_result predefined_assert_not_null(toy_interp *interp, const toy_val_list *args)
{
    assert(val_list_len(args) == 1);
    const toy_val *arg1 = &args->val;
    if (vals_nequal(arg1, &null_val)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should not be null", 1, arg1);
    }
    return REACHED_BLOCK_END;
}

static run_stmt_result predefined_assert(toy_interp *interp, const toy_val_list *args)
{
    assert(val_list_len(args) == 1);
    assert(!args->next);
    const toy_val *arg = &args->val;
    toy_bool b = val_truthy(arg);
    if (b) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be true", 1, arg);
    }
    return REACHED_BLOCK_END;
}

typedef struct val_list_all_args_struct {
    toy_interp *interp;
    toy_function *func;
} val_list_all_args;

static toy_bool val_list_all_callback(void *cookie, size_t index, const toy_val_list *list)
{
    val_list_all_args *args = (val_list_all_args *) cookie;
    const toy_val *value = val_list_payload_const(list);
    run_stmt_result res = interp_run_func_single_arg(args->interp, args->func, value);
    (void) res; /* ignore reason for user function exit */
    return val_truthy(value);
}

static run_stmt_result predefined_list_all(toy_interp *interp, const toy_val_list *args)
{
    assert(val_list_len(args) == 2);
    toy_bool ret;
    const toy_val *arg1 = &args->val;
    const toy_val *arg2 = &args->next->val;
    if (arg1->type == VAL_LIST) {
        toy_val_list *list = arg1->list;
        if (arg2->type == VAL_FUNC) {
            toy_function *func = arg2->func;
            val_list_all_args args = { .func = func, .interp = interp };
            ret = val_list_all_match(list, val_list_all_callback, &args);
        } else {
            invalid_argument_type(VAL_FUNC, arg2);
        }
    } else {
        invalid_argument_type(VAL_LIST, arg1);
    }
    return ret;
}

typedef struct val_list_foreach_args_struct {
    toy_interp *interp;
    toy_function *toy_func;
} val_list_foreach_args;

static item_callback_result val_list_foreach_item_callback(void *cookie, size_t index, const toy_val_list *list)
{
    val_list_foreach_args *args = (val_list_foreach_args *) cookie;
    const toy_val *value = val_list_payload_const(list);
    run_stmt_result res = interp_run_func_single_arg(args->interp, args->toy_func, value);
    (void) res; /* ignore reason for user function exit */
    /* TODO: Use val_truthy to allow the user function to return a truthy value to terminate enumeration */
    return CONTINUE_ENUMERATION;
}

static run_stmt_result predefined_list_foreach(toy_interp *interp, const toy_val_list *args)
{
    assert(val_list_len(args) == 2);
    const toy_val *arg1 = &args->val;
    const toy_val *arg2 = &args->next->val;
    if (arg1->type == VAL_LIST) {
        toy_val_list *list = arg1->list;
        if (arg2->type == VAL_FUNC) {
            toy_function *func = arg2->func;
            val_list_foreach_args cbargs = { .toy_func = func, .interp = interp };
            enumeration_result res = val_list_foreach_const(list, val_list_foreach_item_callback, &cbargs);
            assert(res == ENUMERATION_COMPLETE);
        } else {
            invalid_argument_type(VAL_FUNC, arg2);
        }
    } else {
        invalid_argument_type(VAL_LIST, arg1);
    }
    /* TODO: Return count of items enumerated, or something else meaningful? */
    return REACHED_BLOCK_END;
}

typedef struct val_list_filter_args_struct {
    val_list_foreach_args foreach_args;
    toy_val_list *list_to_append_to;
} val_list_filter_args;

static item_callback_result val_list_filter_item_callback(void *cookie, size_t index, const toy_val_list *list)
{
    val_list_filter_args *args = (val_list_filter_args *) cookie;
    const toy_val *list_elem = val_list_payload_const(list);
    val_assert_valid(list_elem);
    /* TODO: This aliases the arg. Does that allow the user function to modify the value that gets appended? */
    run_stmt_result res = interp_run_func_single_arg(args->foreach_args.interp, args->foreach_args.toy_func, list_elem);
    toy_bool truthy_return;
    if (res == REACHED_RETURN) {
        toy_val *return_value = interp_get_return_value(args->foreach_args.interp);
        val_assert_valid(return_value);
        truthy_return = val_truthy(return_value);
    } else {
        truthy_return = TOY_FALSE;
    }
    if (truthy_return) {
        val_list_assert_valid(args->list_to_append_to);
        if (args->list_to_append_to == NULL) {
            args->list_to_append_to = val_list_alloc(list_elem);
        } else {
            args->list_to_append_to = val_list_append(args->list_to_append_to, list_elem);
        }
        val_list_assert_valid(args->list_to_append_to);
    }
    return CONTINUE_ENUMERATION;
}

static run_stmt_result predefined_list_filter(toy_interp *interp, const toy_val_list *args)
{
    assert(val_list_len(args) == 2);
    const toy_val *arg1 = &args->val;
    const toy_val *arg2 = &args->next->val;
    if (arg1->type == VAL_LIST) {
        toy_val_list *list = arg1->list;
        if (arg2->type == VAL_FUNC) {
            toy_function *user_func = arg2->func;
            /* TODO: Use val_list_find_all_const */
            toy_val list_to_return = { .type = VAL_LIST, .list = NULL };
            val_list_filter_args cb_args = { .list_to_append_to = list_to_return.list, .foreach_args.interp = interp, .foreach_args.toy_func = user_func };
            enumeration_result res = val_list_foreach_const(list, val_list_filter_item_callback, &cb_args);
            assert(ENUMERATION_COMPLETE == res);
            assert(list_to_return.type == VAL_LIST);
            val_list_assert_valid(list_to_return.list);
            interp_set_return_value(interp, &list_to_return);
        } else {
            invalid_argument_type(VAL_FUNC, arg2);
        }
    } else {
        invalid_argument_type(VAL_LIST, arg1);
    }
    return REACHED_RETURN;
}

static toy_bool val_list_none_callback(void *cookie, size_t index, const toy_val_list *list)
{
    val_list_all_args *args = (val_list_all_args *) cookie;
    const toy_val *value = val_list_payload_const(list);
    run_stmt_result res = interp_run_func_single_arg(args->interp, args->func, value);
    (void) res; /* ignore reason for user function exit */
    return val_truthy(value);
}

static run_stmt_result predefined_list_none(toy_interp *interp, const toy_val_list *args)
{
    assert(val_list_len(args) == 2);
    toy_bool ret;
    const toy_val *arg1 = &args->val;
    const toy_val *arg2 = &args->next->val;
    if (arg1->type == VAL_LIST) {
        toy_val_list *list = arg1->list;
        if (arg2->type == VAL_FUNC) {
            toy_function *func = arg2->func;
            val_list_all_args args = { .func = func, .interp = interp };
            ret = val_list_none_match(list, val_list_none_callback, &args);
        } else {
            invalid_argument_type(VAL_FUNC, arg2);
        }
    } else {
        invalid_argument_type(VAL_LIST, arg1);
    }
    return ret;
}

typedef struct map_foreach_args_struct {
    toy_interp *interp;
    toy_function *func;
} map_foreach_args;

static item_callback_result map_foreach_callback(void *cookie, const map_val_entry *entry)
{
    map_foreach_args *args = (map_foreach_args *) cookie;
    const toy_val key_val = { .type = VAL_STR, .str = entry->key };
    const toy_val_list value_arg = { .val = entry->value, .next = NULL };
    const toy_val_list func_args = { .val = key_val, .next = (toy_val_list *) &value_arg };
    interp_run_func_val_list(args->interp, args->func, &func_args);
    /* TODO: Allow the user function to return a truthy value to terminate enumeration */
    return CONTINUE_ENUMERATION;
}

/* TODO: Early exit if user-defined callback returns true */
static run_stmt_result predefined_map_foreach(toy_interp *interp, const toy_val_list *args)
{
    assert(val_list_len(args) == 2);
    const toy_val *arg1 = &args->val;
    const toy_val *arg2 = &args->next->val;
    if (arg1->type == VAL_MAP) {
        map_val *map = arg1->map;
        if (arg2->type == VAL_FUNC) {
            toy_function *func = arg2->func;
            map_foreach_args cbargs = { .func = func, .interp = interp };
            enumeration_result res = map_val_foreach_const(map, map_foreach_callback, &cbargs);
            assert(res == ENUMERATION_COMPLETE);
        } else {
            invalid_argument_type(VAL_FUNC, arg2);
        }
    } else {
        invalid_argument_type(VAL_LIST, arg1);
    }
    /* TODO: Return some kind of meaningful value, such as whether enumeration completed? */
    return REACHED_BLOCK_END;
}

typedef struct map_filter_cb_args_struct {
    map_foreach_args foreach_args;
    map_val *map_to_insert_into;
} map_filter_cb_args;

static item_callback_result map_filter_callback(void *cookie, const map_val_entry *entry)
{
    val_assert_valid(&entry->value);
    map_filter_cb_args *args = (map_filter_cb_args *) cookie;
    map_val_assert_valid(args->map_to_insert_into);
    const toy_val key_val = { .type = VAL_STR, .str = entry->key };
    /* TODO: This aliases the args. Does that allow the user function to modify the value that gets inserted? */
    const toy_val_list func_arg_2 = { .val = entry->value, .next = NULL };
    const toy_val_list func_args = { .val = key_val, .next = (toy_val_list *) &func_arg_2 };
    run_stmt_result res = interp_run_func_val_list(args->foreach_args.interp, args->foreach_args.func, &func_args);
    toy_bool truthy_return;
    if (res == REACHED_RETURN) {
        toy_val *return_value = interp_get_return_value(args->foreach_args.interp);
        val_assert_valid(return_value);
        truthy_return = val_truthy(return_value);
    } else {
        truthy_return = TOY_FALSE;
    }
    if (truthy_return) {
        assert(args->map_to_insert_into != NULL);
        set_result set_res = map_val_set(args->map_to_insert_into, entry->key, &entry->value);
        assert(SET_NEW == set_res);
        map_val_assert_valid(args->map_to_insert_into);
    }
    return CONTINUE_ENUMERATION;
}

static run_stmt_result predefined_map_filter(toy_interp *interp, const toy_val_list *args)
{
    assert(val_list_len(args) == 2);
    const toy_val *arg1 = &args->val;
    const toy_val *arg2 = &args->next->val;
    if (arg1->type == VAL_MAP) {
        map_val *map = arg1->map;
        map_val_assert_valid(map);
        if (arg2->type == VAL_FUNC) {
            toy_function *func = arg2->func;
            toy_val map_to_return = { .type = VAL_MAP, .map = map_val_alloc() };
            map_val_assert_valid(map_to_return.map);
            map_filter_cb_args filter_args = { .foreach_args.func = func, .foreach_args.interp = interp, .map_to_insert_into = map_to_return.map };
            enumeration_result res = map_val_foreach_const(map, map_filter_callback, &filter_args);
            assert(res == ENUMERATION_COMPLETE);
            assert(map_to_return.type == VAL_MAP);
            map_val_assert_valid(map_to_return.map);
            interp_set_return_value(interp, &map_to_return);
        } else {
            invalid_argument_type(VAL_FUNC, arg2);
        }
    } else {
        invalid_argument_type(VAL_LIST, arg1);
    }
    return REACHED_RETURN;
}

/* TODO: These should accept toy_exprs, so their types can be statically validated */
const toy_str_list INFINITE_PARAMS;
static const toy_str_list assert_binary_param_2 = { .str = "val2", .next = NULL };
static const toy_str_list assert_binary_params = { .str = "val1", .next = (toy_str_list *) &assert_binary_param_2 };
static const toy_str_list assert_unary_params = { .str = "val", .next = NULL };
static const toy_str_list list_all_param_2 = { .str = "func", .next = NULL };
static const toy_str_list list_all_params = { .str = "list", .next = (toy_str_list *) &list_all_param_2 };
static const toy_str_list list_foreach_param_2 = { .str = "func", .next = NULL };
static const toy_str_list list_foreach_params = { .str = "list", .next = (toy_str_list *) &list_foreach_param_2 };
static const toy_str_list list_filter_param_2 = { .str = "func", .next = NULL };
static const toy_str_list list_filter_params = { .str = "list", .next = (toy_str_list *) &list_filter_param_2 };
static const toy_str_list list_len_params = { .str = "list", .next = NULL };
static const toy_str_list list_none_param_2 = { .str = "func", .next = NULL };
static const toy_str_list list_none_params = { .str = "list", .next = (toy_str_list *) &list_none_param_2 };
static const toy_str_list map_foreach_param_2 = { .str = "func", .next = NULL };
static const toy_str_list map_foreach_params = { .str = "map", .next = (toy_str_list *) &map_foreach_param_2 };
static const toy_str_list map_filter_param_2 = { .str = "func", .next = NULL };
static const toy_str_list map_filter_params = { .str = "map", .next = (toy_str_list *) &map_filter_param_2 };
static const toy_str_list map_len_params = { .str = "map", .next = NULL };

static const toy_function func_assert           = { .parent = NULL, .name = "assert",           .type = FUNC_PREDEFINED, .predef = predefined_assert,           .param_names = (toy_str_list *) &assert_unary_params,  .doc = "Assert that a givel value is truthy. Fail if it is not." };
static const toy_function func_assert_equal     = { .parent = NULL, .name = "assert_equal",     .type = FUNC_PREDEFINED, .predef = predefined_assert_equal,     .param_names = (toy_str_list *) &assert_binary_params, .doc = "Assert that two values are equal. Fail if they are not." };
static const toy_function func_assert_gt        = { .parent = NULL, .name = "assert_gt",        .type = FUNC_PREDEFINED, .predef = predefined_assert_gt,        .param_names = (toy_str_list *) &assert_binary_params, .doc = "Assert that the first value is strictly greater than the second. Fail if it is not." };
static const toy_function func_assert_gte       = { .parent = NULL, .name = "assert_gte",       .type = FUNC_PREDEFINED, .predef = predefined_assert_gte,       .param_names = (toy_str_list *) &assert_binary_params, .doc = "Assert that the first value is greater than or equal to the second. Fail if it is not." };
static const toy_function func_assert_lt        = { .parent = NULL, .name = "assert_lt",        .type = FUNC_PREDEFINED, .predef = predefined_assert_lt,        .param_names = (toy_str_list *) &assert_binary_params, .doc = "Assert that the first value is strictly less than the second. Fail if it is not." };
static const toy_function func_assert_lte       = { .parent = NULL, .name = "assert_lte",       .type = FUNC_PREDEFINED, .predef = predefined_assert_lte,       .param_names = (toy_str_list *) &assert_binary_params, .doc = "Assert that the first value is less than or equal to the second. Fail if it is not." };
static const toy_function func_assert_not_equal = { .parent = NULL, .name = "assert_not_equal", .type = FUNC_PREDEFINED, .predef = predefined_assert_not_equal, .param_names = (toy_str_list *) &assert_binary_params, .doc = "Assert that two values are not equal. Fail if they are." };
static const toy_function func_assert_not_null  = { .parent = NULL, .name = "assert_not_null",  .type = FUNC_PREDEFINED, .predef = predefined_assert_not_null,  .param_names = (toy_str_list *) &assert_unary_params,  .doc = "Assert that a value is not null. Fail if it is." };
static const toy_function func_assert_not_zero  = { .parent = NULL, .name = "assert_not_zero",  .type = FUNC_PREDEFINED, .predef = predefined_assert_not_zero,  .param_names = (toy_str_list *) &assert_unary_params,  .doc = "Assert that a value is not zero. Fail if it is." };
static const toy_function func_assert_null      = { .parent = NULL, .name = "assert_null",      .type = FUNC_PREDEFINED, .predef = predefined_assert_null,      .param_names = (toy_str_list *) &assert_unary_params,  .doc = "Assert that a value is null. Fail if it is not."};
static const toy_function func_assert_zero      = { .parent = NULL, .name = "assert_zero",      .type = FUNC_PREDEFINED, .predef = predefined_assert_zero,      .param_names = (toy_str_list *) &assert_unary_params,  .doc = "Assert that a value is zero. Fail if it is not." };
static const toy_function func_list_all         = { .parent = NULL, .name = "list_all",         .type = FUNC_PREDEFINED, .predef = predefined_list_all,         .param_names = (toy_str_list *) &list_all_params,      .doc = "Return true if the given function returns a truthy value when called with each item in the given list." };
static const toy_function func_list_len         = { .parent = NULL, .name = "list_len",         .type = FUNC_PREDEFINED, .predef = predefined_list_len,         .param_names = (toy_str_list *) &list_len_params,      .doc = "Count the number of items in the given list." };
static const toy_function func_list_foreach     = { .parent = NULL, .name = "list_foreach",     .type = FUNC_PREDEFINED, .predef = predefined_list_foreach,     .param_names = (toy_str_list *) &list_foreach_params,  .doc = "Call the given function once with each item of the given list." };
static const toy_function func_list_filter      = { .parent = NULL, .name = "list_filter",      .type = FUNC_PREDEFINED, .predef = predefined_list_filter,      .param_names = (toy_str_list *) &list_filter_params,   .doc = "Call the first function once with each item of the given list. If it returns a truthy value, call the second function with it." };
static const toy_function func_list_none        = { .parent = NULL, .name = "list_none",        .type = FUNC_PREDEFINED, .predef = predefined_list_none,        .param_names = (toy_str_list *) &list_none_params,     .doc = "Return true if the given function returns a falsy value when called with each item in the given list." };
/* TODO: map_all */
static const toy_function func_map_foreach      = { .parent = NULL, .name = "map_foreach",      .type = FUNC_PREDEFINED, .predef = predefined_map_foreach,      .param_names = (toy_str_list *) &map_foreach_params,   .doc = "Call the given function once with each (name, value) entry in the given map." };
static const toy_function func_map_filter       = { .parent = NULL, .name = "map_filter",       .type = FUNC_PREDEFINED, .predef = predefined_map_filter,       .param_names = (toy_str_list *) &map_filter_params,    .doc = "Call the first function once with each (name, value) entry in the given map. If it returns a truthy value, call the second function with the same entry." };
static const toy_function func_map_len          = { .parent = NULL, .name = "map_len",          .type = FUNC_PREDEFINED, .predef = predefined_map_len,          .param_names = (toy_str_list *) &map_len_params,       .doc = "Count the number of entries in the given map." };
/* TODO: map_none */
static const toy_function func_print            = { .parent = NULL, .name = "print",            .type = FUNC_PREDEFINED, .predef = predefined_print,            .param_names = (toy_str_list *) &INFINITE_PARAMS,      .doc = "Output the given message to the console." };

static const toy_val predef_functions[] = {
    { .type = VAL_FUNC, .func = (toy_function *) &func_assert },
    { .type = VAL_FUNC, .func = (toy_function *) &func_assert_equal },
    { .type = VAL_FUNC, .func = (toy_function *) &func_assert_gt },
    { .type = VAL_FUNC, .func = (toy_function *) &func_assert_gte },
    { .type = VAL_FUNC, .func = (toy_function *) &func_assert_lt },
    { .type = VAL_FUNC, .func = (toy_function *) &func_assert_lte },
    { .type = VAL_FUNC, .func = (toy_function *) &func_assert_not_equal },
    { .type = VAL_FUNC, .func = (toy_function *) &func_assert_not_null },
    { .type = VAL_FUNC, .func = (toy_function *) &func_assert_not_zero },
    { .type = VAL_FUNC, .func = (toy_function *) &func_assert_null },
    { .type = VAL_FUNC, .func = (toy_function *) &func_assert_zero },
    { .type = VAL_FUNC, .func = (toy_function *) &func_list_all },
    { .type = VAL_FUNC, .func = (toy_function *) &func_list_len },
    { .type = VAL_FUNC, .func = (toy_function *) &func_list_foreach },
    { .type = VAL_FUNC, .func = (toy_function *) &func_list_filter },
    { .type = VAL_FUNC, .func = (toy_function *) &func_list_none },
    { .type = VAL_FUNC, .func = (toy_function *) &func_map_foreach },
    { .type = VAL_FUNC, .func = (toy_function *) &func_map_filter },
    { .type = VAL_FUNC, .func = (toy_function *) &func_map_len },
    { .type = VAL_FUNC, .func = (toy_function *) &func_print }
};

static int compare_function_names(const void *p1, const void *p2)
{
    const toy_val *val1 = p1, *val2 = p2;
    return strcmp(val1->func->name, val2->func->name);
}

const toy_val *predef_func_lookup_name(const toy_str name)
{
    toy_function look_for_func = { .name = name };
    toy_val look_for_val = { .func = &look_for_func };
    return bsearch(&look_for_val, predef_functions, ELEMENTSOF(predef_functions), sizeof(predef_functions[0]), compare_function_names);
}
