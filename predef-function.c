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
#include "constant.h"
#include "log.h"
#include "var.h"
#include "list-visitor.h"
#include "map-visitor.h"
#include "list-filter.h"
#include "map-filter.h"
#include "generic-map.h"
#include "function.h"
#include "str-list.h"

static run_stmt_result predefined_list_len(toy_interp *interp, const toy_var *args, size_t num_args)
{
    assert(args);
    assert(num_args == 1);
    const toy_val *arg = var_get_const(&args[0]);
    if (arg->type != VAL_LIST) {
        invalid_argument_type(VAL_LIST, arg);
    }
    toy_val *return_val = interp_get_return_value(interp);
    return_val->type = VAL_NUM;
    assert(arg->type == VAL_LIST);
    return_val->num = val_list_len(arg->list);
    return REACHED_RETURN;
}

static run_stmt_result predefined_obj_len(toy_interp *interp, const toy_var *args, size_t num_args)
{
    assert(args);
    assert(num_args == 1);
    const toy_val *arg = var_get_const(&args[0]);
    if (arg->type != VAL_MAP) {
        invalid_argument_type(VAL_MAP, arg);
    }
    toy_val *return_val = interp_get_return_value(interp);
    return_val->type = VAL_NUM;
    assert(arg->type == VAL_MAP);
    return_val->num = map_val_size(arg->obj);
    return REACHED_RETURN;
}

typedef struct map_keys_visitor_struct {
    const_map_visitor map_vis;
    toy_val_list *result;
} map_keys_visitor;

static item_callback_result obj_keys_visit_entry(map_keys_visitor *map_keys_vis, const generic_map_entry *entry)
{
    toy_val val = { .type = VAL_STR, .str = entry->key };
    if (map_keys_vis->result) {
        map_keys_vis->result = val_list_append(map_keys_vis->result, &val);
    } else {
        map_keys_vis->result = val_list_alloc(&val);
    }
    return CONTINUE_ENUMERATION;
}

static run_stmt_result predefined_obj_keys(toy_interp *interp, const toy_var *args, size_t num_args)
{
    assert(args);
    assert(num_args == 1);
    const toy_val *arg = var_get_const(&args[0]);
    if (arg->type != VAL_MAP) {
        invalid_argument_type(VAL_MAP, arg);
    }
    map_val *map = arg->obj;
    map_keys_visitor map_keys_vis = { .map_vis.visit_entry = (const_map_entry_visit_func) obj_keys_visit_entry, .result = NULL };
    enumeration_result res = const_map_visitor_visit_map((const_map_visitor *) &map_keys_vis, (const generic_map *) map);
    assert(ENUMERATION_COMPLETE == res);
    toy_val return_val = { .type = VAL_LIST, .list = map_keys_vis.result };
    interp_set_return_value(interp, &return_val);
    return REACHED_RETURN;
}

static run_stmt_result predefined_print(toy_interp *interp, const toy_var *args, size_t num_args)
{
    for (const toy_var *var = args; var < &args[num_args]; var++) {
        const toy_val *val = var_get_const(var);
        /* TODO: This should output to stdout, not the log stream going to stderr */
        if (val->type == VAL_STR) {
            print_str(val->str);
        } else {
            val_dump(val, 1);
        }
        log_putc(LOG_DEBUG, '\n');
    }
    return REACHED_BLOCK_END;
}

static void toy_assert_fail(const char * msg, size_t num_vals, ...)
{
    va_list argptr;
    va_start(argptr, num_vals);
    while (num_vals--) {
        const toy_val *val = va_arg(argptr, const toy_val *);
        val_dump(val, 1);
        log_putc(LOG_DEBUG, '\n');
    }
    va_end(argptr);
    fatal_error("Assertion failed: %s", msg);
}

static run_stmt_result predefined_assert_equal(toy_interp *interp, const toy_var *args, size_t num_args)
{
    assert(num_args == 2);
    const toy_val *arg1 = var_get_const(&args[0]);
    const toy_val *arg2 = var_get_const(&args[1]);
    if (vals_equal(arg1, arg2)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be equal", 2, arg1, arg2);
    }
    return REACHED_BLOCK_END;
}

static run_stmt_result predefined_assert_not_equal(toy_interp *interp, const toy_var *args, size_t num_args)
{
    assert(num_args == 2);
    const toy_val *arg1 = var_get_const(&args[0]);
    const toy_val *arg2 = var_get_const(&args[1]);
    if (vals_nequal(arg1, arg2)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should not be equal", 2, arg1, arg2);
    }
    return REACHED_BLOCK_END;
}

static run_stmt_result predefined_assert_gt(toy_interp *interp, const toy_var *args, size_t num_args)
{
    assert(num_args == 2);
    const toy_val *arg1 = var_get_const(&args[0]);
    const toy_val *arg2 = var_get_const(&args[1]);
    if (val_gt(arg1, arg2)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be greater than", 2, arg1, arg2);
    }
    return REACHED_BLOCK_END;
}

static run_stmt_result predefined_assert_gte(toy_interp *interp, const toy_var *args, size_t num_args)
{
    assert(num_args == 2);
    const toy_val *arg1 = var_get_const(&args[0]);
    const toy_val *arg2 = var_get_const(&args[1]);
    if (val_gte(arg1, arg2)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be greater than or equal", 2, arg1, arg2);
    }
    return REACHED_BLOCK_END;
}

static run_stmt_result predefined_assert_lt(toy_interp *interp, const toy_var *args, size_t num_args)
{
    assert(num_args == 2);
    const toy_val *arg1 = var_get_const(&args[0]);
    const toy_val *arg2 = var_get_const(&args[1]);
    if (val_lt(arg1, arg2)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be less than", 2, arg1, arg2);
    }
    return REACHED_BLOCK_END;
}

static run_stmt_result predefined_assert_lte(toy_interp *interp, const toy_var *args, size_t num_args)
{
    assert(num_args == 2);
    const toy_val *arg1 = var_get_const(&args[0]);
    const toy_val *arg2 = var_get_const(&args[1]);
    if (val_lte(arg1, arg2)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be less than or equal", 2, arg1, arg2);
    }
    return REACHED_BLOCK_END;
}

static const toy_val zero = { .type = VAL_NUM, .num = 0 };

static run_stmt_result predefined_assert_zero(toy_interp *interp, const toy_var *args, size_t num_args)
{
    assert(num_args == 1);
    const toy_val *arg = var_get_const(&args[0]);
    if (vals_equal(arg, &zero)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be zero", 1, arg);
    }
    return REACHED_BLOCK_END;
}

static run_stmt_result predefined_assert_not_zero(toy_interp *interp, const toy_var *args, size_t num_args)
{
    assert(num_args == 1);
    const toy_val *arg = var_get_const(&args[0]);
    if (vals_nequal(arg, &zero)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be non-zero", 1, arg);
    }
    return REACHED_BLOCK_END;
}

static run_stmt_result predefined_assert_null(toy_interp *interp, const toy_var *args, size_t num_args)
{
    assert(num_args == 1);
    const toy_val *arg = var_get_const(&args[0]);
    if (vals_equal(arg, &null_val)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be null", 1, arg);
    }
    return REACHED_BLOCK_END;
}

static run_stmt_result predefined_assert_not_null(toy_interp *interp, const toy_var *args, size_t num_args)
{
    assert(num_args == 1);
    const toy_val *arg = var_get_const(&args[0]);
    if (vals_nequal(arg, &null_val)) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should not be null", 1, arg);
    }
    return REACHED_BLOCK_END;
}

static run_stmt_result predefined_assert(toy_interp *interp, const toy_var *args, size_t num_args)
{
    assert(num_args == 1);
    const toy_val *arg = var_get_const(&args[0]);
    toy_bool b = val_truthy(arg);
    if (b) {
        /* Assertion succeeded */
    } else {
        toy_assert_fail("Should be true", 1, arg);
    }
    return REACHED_BLOCK_END;
}

typedef struct user_func_predicate_struct {
    toy_interp *interp;
    toy_function *func;
} user_func_predicate;

static toy_bool val_list_test_predicate(user_func_predicate *predicate, size_t index, const toy_val_list *list)
{
    const toy_val *value = val_list_payload_const(list);
    run_stmt_result run_res = interp_run_func_single_arg(predicate->interp, predicate->func, value);
    switch (run_res) {
    case REACHED_BLOCK_END:
        no_return_value(predicate->func);
        break;
    case REACHED_BREAK:
    case REACHED_CONTINUE:
        assert(0);
        break;
    case REACHED_RETURN:
        toy_val *return_value = interp_get_return_value(predicate->interp);
        return val_truthy(return_value);
    default:
        assert(0);
        break;
    }
    assert(0);
    return TOY_FALSE;
}

static run_stmt_result predefined_list_all(toy_interp *interp, const toy_var *args, size_t num_args)
{
    assert(num_args == 2);
    const toy_val *arg1 = var_get_const(&args[0]);
    const toy_val *arg2 = var_get_const(&args[1]);
    toy_bool ret;
    if (arg1->type == VAL_LIST) {
        toy_val_list *list = arg1->list;
        if (arg2->type == VAL_FUNC) {
            toy_function *func = arg2->func;
            user_func_predicate val_list_pred = { .func = func, .interp = interp };
            ret = list_all_match((const generic_list *) list, (generic_list_filter_func) val_list_test_predicate, &val_list_pred);
        } else {
            invalid_argument_type(VAL_FUNC, arg2);
        }
    } else {
        invalid_argument_type(VAL_LIST, arg1);
    }
    toy_val return_val = { .type = VAL_BOOL, .boolean = ret };
    interp_set_return_value(interp, &return_val);
    return REACHED_RETURN; 
}

static run_stmt_result predefined_list_not_all(toy_interp *interp, const toy_var *args, size_t num_args)
{
    assert(num_args == 2);
    const toy_val *arg1 = var_get_const(&args[0]);
    const toy_val *arg2 = var_get_const(&args[1]);
    toy_bool ret;
    if (arg1->type == VAL_LIST) {
        toy_val_list *list = arg1->list;
        if (arg2->type == VAL_FUNC) {
            toy_function *func = arg2->func;
            user_func_predicate val_list_pred = { .func = func, .interp = interp };
            ret = list_not_all_match((const generic_list *) list, (generic_list_filter_func) val_list_test_predicate, &val_list_pred);
        } else {
            invalid_argument_type(VAL_FUNC, arg2);
        }
    } else {
        invalid_argument_type(VAL_LIST, arg1);
    }
    toy_val return_val = { .type = VAL_BOOL, .boolean = ret };
    interp_set_return_value(interp, &return_val);
    return REACHED_RETURN; 
}

static run_stmt_result predefined_list_some(toy_interp *interp, const toy_var *args, size_t num_args)
{
    assert(num_args == 2);
    const toy_val *arg1 = var_get_const(&args[0]);
    const toy_val *arg2 = var_get_const(&args[1]);
    toy_bool ret;
    if (arg1->type == VAL_LIST) {
        toy_val_list *list = arg1->list;
        if (arg2->type == VAL_FUNC) {
            toy_function *func = arg2->func;
            user_func_predicate val_list_pred = { .func = func, .interp = interp };
            ret = list_some_match((const generic_list *) list, (generic_list_filter_func) val_list_test_predicate, &val_list_pred);
        } else {
            invalid_argument_type(VAL_FUNC, arg2);
        }
    } else {
        invalid_argument_type(VAL_LIST, arg1);
    }
    toy_val return_val = { .type = VAL_BOOL, .boolean = ret };
    interp_set_return_value(interp, &return_val);
    return REACHED_RETURN; 
}

static run_stmt_result predefined_list_none(toy_interp *interp, const toy_var *args, size_t num_args)
{
    assert(num_args == 2);
    const toy_val *arg1 = var_get_const(&args[0]);
    const toy_val *arg2 = var_get_const(&args[1]);
    toy_bool ret;
    if (arg1->type == VAL_LIST) {
        toy_val_list *list = arg1->list;
        if (arg2->type == VAL_FUNC) {
            toy_function *func = arg2->func;
            user_func_predicate val_list_pred = { .func = func, .interp = interp };
            ret = list_none_match((const generic_list *) list, (generic_list_filter_func) val_list_test_predicate, &val_list_pred);
        } else {
            invalid_argument_type(VAL_FUNC, arg2);
        }
    } else {
        invalid_argument_type(VAL_LIST, arg1);
    }
    toy_val return_val = { .type = VAL_BOOL, .boolean = ret };
    interp_set_return_value(interp, &return_val);
    return REACHED_RETURN; 
}

typedef struct val_list_foreach_visitor_struct {
    const_list_visitor list_vis;
    toy_interp *interp;
    toy_function *toy_func;
    size_t num_seen;
} val_list_foreach_visitor;

static item_callback_result val_list_foreach_item_callback(val_list_foreach_visitor *val_list_vis, size_t index, const toy_val_list *list)
{
    const toy_val *value = val_list_payload_const(list);
    val_list_vis->num_seen++;
    run_stmt_result run_result = interp_run_func_single_arg(val_list_vis->interp, val_list_vis->toy_func, value);
    if (run_result == REACHED_RETURN) {
    }
    switch (run_result) {
    case REACHED_BLOCK_END:
        /* Continue enumerating by default */
        return CONTINUE_ENUMERATION;
    case REACHED_BREAK:
    case REACHED_CONTINUE:
        assert(0);
        break;
    case REACHED_RETURN:
        toy_val *return_value = interp_get_return_value(val_list_vis->interp);
        if (val_falsey(return_value)) {
            return STOP_ENUMERATION;
        }
        return CONTINUE_ENUMERATION;
    default:
        assert(0);
        break;
    }
    assert(0);
    return CONTINUE_ENUMERATION;
}

static run_stmt_result predefined_list_foreach(toy_interp *interp, const toy_var *args, size_t num_args)
{
    assert(num_args == 2);
    const toy_val *arg1 = var_get_const(&args[0]);
    const toy_val *arg2 = var_get_const(&args[1]);
    if (arg1->type == VAL_LIST) {
        const toy_val_list *list = arg1->list;
        if (arg2->type == VAL_FUNC) {
            toy_function *func = arg2->func;
            val_list_foreach_visitor val_list_vis = {
                .list_vis.visit_entry = (const_list_entry_visit_func) val_list_foreach_item_callback,
                .toy_func = func,
                .interp = interp,
                .num_seen = 0
            };
            enumeration_result res = const_list_visitor_visit_list((const_list_visitor *) &val_list_vis, (const generic_list *) list);
            assert(res == ENUMERATION_COMPLETE || res == ENUMERATION_INTERRUPTED);
            toy_val return_value = { .type = VAL_NUM, .num = val_list_vis.num_seen };
            interp_set_return_value(interp, &return_value);
        } else {
            invalid_argument_type(VAL_FUNC, arg2);
        }
    } else {
        invalid_argument_type(VAL_LIST, arg1);
    }
    return REACHED_BLOCK_END;
}

typedef struct val_list_map_visitor_struct {
    const_list_visitor list_vis;
    toy_interp *interp;
    toy_function *toy_func;
    toy_val_list *result;
} val_list_map_visitor;

static item_callback_result val_list_map_item_callback(val_list_map_visitor *val_list_vis, size_t index, const toy_val_list *list)
{
    const toy_val *value = val_list_payload_const(list);

    toy_function *func = val_list_vis->toy_func;
    if (func->param_names && func->param_names != &INFINITE_PARAMS) {
        size_t params_len = str_list_len(func->param_names);
        if (params_len != 1) {
            incorrect_function_num_args(func, 1);
        }
    }
    run_stmt_result run_result = interp_run_func_single_arg(val_list_vis->interp, func, value);
    const toy_val *return_value;
    if (run_result == REACHED_RETURN) {
        return_value = interp_get_return_value(val_list_vis->interp);
    } else {
        return_value = &null_val;
    }
    if (val_list_vis->result) {
        val_list_vis->result = val_list_append(val_list_vis->result, return_value);
    } else {
        val_list_vis->result = val_list_alloc(return_value);
    }
    return CONTINUE_ENUMERATION;
}

static run_stmt_result predefined_list_map(toy_interp *interp, const toy_var *args, size_t num_args)
{
    assert(num_args == 2);
    const toy_val *arg1 = var_get_const(&args[0]);
    const toy_val *arg2 = var_get_const(&args[1]);
    if (arg1->type == VAL_LIST) {
        const toy_val_list *list = arg1->list;
        if (arg2->type == VAL_FUNC) {
            toy_function *func = arg2->func;
            val_list_map_visitor val_list_vis = {
                .list_vis.visit_entry = (const_list_entry_visit_func) val_list_map_item_callback,
                .toy_func = func,
                .interp = interp,
                .result = NULL
            };
            if (list) {
                enumeration_result res = const_list_visitor_visit_list((const_list_visitor *) &val_list_vis, (const generic_list *) list);
                assert(res == ENUMERATION_COMPLETE || res == ENUMERATION_INTERRUPTED);
            }
            toy_val return_value = { .type = VAL_LIST, .list = val_list_vis.result };
            interp_set_return_value(interp, &return_value);
        } else {
            invalid_argument_type(VAL_FUNC, arg2);
        }
    } else {
        invalid_argument_type(VAL_LIST, arg1);
    }
    return REACHED_RETURN;
}

/* TODO: Use list_filter */
typedef struct val_list_filter_struct {
    val_list_foreach_visitor val_list_vis;
    toy_val_list *list_to_append_to;
} val_list_filter;

static item_callback_result val_list_filter_item_callback(val_list_filter *val_list_filt, size_t index, const toy_val_list *list)
{
    const toy_val *list_elem = val_list_payload_const(list);
    val_assert_valid(list_elem);
    /* TODO: This aliases the arg. Does that allow the user function to modify the value that gets appended? */
    run_stmt_result res = interp_run_func_single_arg(val_list_filt->val_list_vis.interp, val_list_filt->val_list_vis.toy_func, list_elem);
    toy_bool truthy_return;
    /* TODO: Error if user function didn't return a value */
    if (res == REACHED_RETURN) {
        toy_val *return_value = interp_get_return_value(val_list_filt->val_list_vis.interp);
        val_assert_valid(return_value);
        truthy_return = val_truthy(return_value);
    } else {
        truthy_return = TOY_FALSE;
    }
    if (truthy_return) {
        val_list_assert_valid(val_list_filt->list_to_append_to);
        if (val_list_filt->list_to_append_to == NULL) {
            val_list_filt->list_to_append_to = val_list_alloc(list_elem);
        } else {
            val_list_filt->list_to_append_to = val_list_append(val_list_filt->list_to_append_to, list_elem);
        }
        val_list_assert_valid(val_list_filt->list_to_append_to);
    }
    return CONTINUE_ENUMERATION;
}

static run_stmt_result predefined_list_filter(toy_interp *interp, const toy_var *args, size_t num_args)
{
    assert(num_args == 2);
    const toy_val *arg1 = var_get_const(&args[0]);
    const toy_val *arg2 = var_get_const(&args[1]);
    if (arg1->type == VAL_LIST) {
        toy_val_list *list = arg1->list;
        if (arg2->type == VAL_FUNC) {
            toy_function *user_func = arg2->func;
            toy_val list_to_return = { .type = VAL_LIST, .list = NULL };
            val_list_filter val_list_filt = {
                .val_list_vis.list_vis.visit_entry = (const_list_entry_visit_func) val_list_filter_item_callback,
                .val_list_vis.interp = interp,
                .val_list_vis.toy_func = user_func,
                .val_list_vis.num_seen = 0,
                .list_to_append_to = list_to_return.list
            };
            enumeration_result res = const_list_visitor_visit_list((const_list_visitor *) &val_list_filt, (const generic_list *) list);
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

typedef struct map_val_foreach_visitor_struct {
    const_map_visitor map_vis;
    toy_interp *interp;
    toy_function *func;
    size_t num_seen;
} map_val_foreach_visitor;

static item_callback_result obj_foreach_callback(map_val_foreach_visitor *map_val_vis, const map_val_entry *entry)
{
    map_val_vis->num_seen++;
    const toy_val key_val = { .type = VAL_STR, .str = entry->key };
    const toy_val_list value_arg = { .val = entry->value, .next = NULL };
    const toy_val_list func_args = { .val = key_val, .next = (toy_val_list *) &value_arg };
    run_stmt_result run_result = interp_run_func_val_list(map_val_vis->interp, map_val_vis->func, &func_args);
    if (run_result == REACHED_RETURN) {
        toy_val *return_value = interp_get_return_value(map_val_vis->interp);
        if (val_falsey(return_value)) {
            return STOP_ENUMERATION;
        }
    }
    return CONTINUE_ENUMERATION;
}

static run_stmt_result predefined_obj_foreach(toy_interp *interp, const toy_var *args, size_t num_args)
{
    assert(num_args == 2);
    const toy_val *arg1 = var_get_const(&args[0]);
    const toy_val *arg2 = var_get_const(&args[1]);
    if (arg1->type == VAL_MAP) {
        map_val *map = arg1->obj;
        if (arg2->type == VAL_FUNC) {
            toy_function *func = arg2->func;
            map_val_foreach_visitor map_val_vis = {
                .map_vis.visit_entry = (const_map_entry_visit_func) obj_foreach_callback,
                .func = func,
                .interp = interp,
                .num_seen = 0
            };
            enumeration_result res = const_map_visitor_visit_map((const_map_visitor *) &map_val_vis, (const generic_map *) map);
            assert(res == ENUMERATION_COMPLETE);
            toy_val return_value = { .type = VAL_NUM, .num = map_val_vis.num_seen };
            interp_set_return_value(interp, &return_value);
        } else {
            invalid_argument_type(VAL_FUNC, arg2);
        }
    } else {
        invalid_argument_type(VAL_LIST, arg1);
    }
    return REACHED_BLOCK_END;
}

typedef struct map_val_map_visitor_struct {
    const_map_visitor map_vis;
    toy_interp *interp;
    toy_function *func;
    map_val *result;
} map_val_map_visitor;

static item_callback_result obj_map_callback(map_val_map_visitor *map_val_vis, const map_val_entry *entry)
{
    const toy_val key_val = { .type = VAL_STR, .str = entry->key };
    const toy_val_list value_arg = { .val = entry->value, .next = NULL };
    const toy_val_list func_args = { .val = key_val, .next = (toy_val_list *) &value_arg };
    toy_function *func = map_val_vis->func;
    if (func->param_names && func->param_names != &INFINITE_PARAMS) {
        size_t params_len = str_list_len(func->param_names);
        if (params_len != 2) {
            incorrect_function_num_args(func, 2);
        }
    }
    run_stmt_result run_result = interp_run_func_val_list(map_val_vis->interp, func, &func_args);
    const toy_val *return_value;
    if (run_result == REACHED_RETURN) {
        return_value = interp_get_return_value(map_val_vis->interp);
    } else {
        return_value = &null_val;
    }
    if (!map_val_vis->result) {
        map_val_vis->result = map_val_alloc();
    }
    map_val_set(map_val_vis->result, entry->key, return_value);
    return CONTINUE_ENUMERATION;
}

static run_stmt_result predefined_obj_map(toy_interp *interp, const toy_var *args, size_t num_args)
{
    assert(num_args == 2);
    const toy_val *arg1 = var_get_const(&args[0]);
    const toy_val *arg2 = var_get_const(&args[1]);
    if (arg1->type == VAL_MAP) {
        map_val *map = arg1->obj;
        if (arg2->type == VAL_FUNC) {
            toy_function *func = arg2->func;
            map_val_map_visitor map_val_vis = {
                .map_vis.visit_entry = (const_map_entry_visit_func) obj_map_callback,
                .func = func,
                .interp = interp,
                .result = NULL
            };
            if (map) {
                enumeration_result res = const_map_visitor_visit_map((const_map_visitor *) &map_val_vis, (const generic_map *) map);
                assert(res == ENUMERATION_COMPLETE);
            }
            toy_val return_value = { .type = VAL_MAP, .obj = map_val_vis.result };
            interp_set_return_value(interp, &return_value);
        } else {
            invalid_argument_type(VAL_FUNC, arg2);
        }
    } else {
        invalid_argument_type(VAL_LIST, arg1);
    }
    return REACHED_RETURN;
}

typedef struct map_filter_cb_args_struct {
    map_val_foreach_visitor map_val_vis;
    map_val *obj_to_insert_into;
} map_filter_cb_args;

static item_callback_result obj_filter_callback(void *cookie, const map_val_entry *entry)
{
    val_assert_valid(&entry->value);
    map_filter_cb_args *args = (map_filter_cb_args *) cookie;
    map_val_assert_valid(args->obj_to_insert_into);
    const toy_val key_val = { .type = VAL_STR, .str = entry->key };
    /* TODO: This aliases the args. Does that allow the user function to modify the value that gets inserted? */
    const toy_val_list func_arg_2 = { .val = entry->value, .next = NULL };
    const toy_val_list func_args = { .val = key_val, .next = (toy_val_list *) &func_arg_2 };
    run_stmt_result res = interp_run_func_val_list(args->map_val_vis.interp, args->map_val_vis.func, &func_args);
    toy_bool truthy_return;
    if (res == REACHED_RETURN) {
        toy_val *return_value = interp_get_return_value(args->map_val_vis.interp);
        val_assert_valid(return_value);
        truthy_return = val_truthy(return_value);
    } else {
        truthy_return = TOY_FALSE;
    }
    if (truthy_return) {
        assert(args->obj_to_insert_into != NULL);
        set_result set_res = map_val_set(args->obj_to_insert_into, entry->key, &entry->value);
        assert(SET_NEW == set_res);
        map_val_assert_valid(args->obj_to_insert_into);
    }
    return CONTINUE_ENUMERATION;
}

static run_stmt_result predefined_obj_filter(toy_interp *interp, const toy_var *args, size_t num_args)
{
    assert(num_args == 2);
    const toy_val *arg1 = var_get_const(&args[0]);
    const toy_val *arg2 = var_get_const(&args[1]);
    if (arg1->type == VAL_MAP) {
        map_val *map = arg1->obj;
        map_val_assert_valid(map);
        if (arg2->type == VAL_FUNC) {
            toy_function *func = arg2->func;
            toy_val map_to_return = { .type = VAL_MAP, .obj = map_val_alloc() };
            map_val_assert_valid(map_to_return.obj);
            map_filter_cb_args filter_args = {
                .map_val_vis.map_vis.visit_entry = (const_map_entry_visit_func) obj_filter_callback,
                .map_val_vis.num_seen = 0,
                .map_val_vis.func = func,
                .map_val_vis.interp = interp,
                .obj_to_insert_into = map_to_return.obj
            };
            enumeration_result res = const_map_visitor_visit_map((const_map_visitor *) &filter_args, (const generic_map *) map);
            assert(res == ENUMERATION_COMPLETE);
            assert(map_to_return.type == VAL_MAP);
            map_val_assert_valid(map_to_return.obj);
            interp_set_return_value(interp, &map_to_return);
        } else {
            invalid_argument_type(VAL_FUNC, arg2);
        }
    } else {
        invalid_argument_type(VAL_LIST, arg1);
    }
    return REACHED_RETURN;
}

static toy_bool obj_val_test_predicate(user_func_predicate *predicate, const map_val_entry *entry)
{
    const toy_val key_val = { .type = VAL_STR, .str = entry->key };
    toy_val_list *func_args = val_list_alloc(&key_val);
    func_args = val_list_append(func_args, &entry->value);
    run_stmt_result run_res = interp_run_func_val_list(predicate->interp, predicate->func, func_args);
    val_list_free(func_args);
    switch (run_res) {
    case REACHED_BLOCK_END:
        no_return_value(predicate->func);
        break;
    case REACHED_BREAK:
    case REACHED_CONTINUE:
        assert(0);
        break;
    case REACHED_RETURN:
        toy_val *return_value = interp_get_return_value(predicate->interp);
        return val_truthy(return_value);
    default:
        assert(0);
        break;
    }
    assert(0);
    return TOY_FALSE;
}

static run_stmt_result predefined_obj_all(toy_interp *interp, const toy_var *args, size_t num_args)
{
    assert(num_args == 2);
    const toy_val *arg1 = var_get_const(&args[0]);
    const toy_val *arg2 = var_get_const(&args[1]);
    toy_bool ret;
    if (arg1->type == VAL_MAP) {
        map_val *map = arg1->obj;
        if (arg2->type == VAL_FUNC) {
            toy_function *func = arg2->func;
            user_func_predicate predicate = { .func = func, .interp = interp };
            ret = map_all_match((generic_map *) map, (generic_map_filter_func) obj_val_test_predicate, &predicate);
        } else {
            invalid_argument_type(VAL_FUNC, arg2);
        }
    } else {
        invalid_argument_type(VAL_MAP, arg1);
    }
    toy_val return_val = { .type = VAL_BOOL, .boolean = ret };
    interp_set_return_value(interp, &return_val);
    return REACHED_RETURN;
}

static run_stmt_result predefined_obj_not_all(toy_interp *interp, const toy_var *args, size_t num_args)
{
    assert(num_args == 2);
    const toy_val *arg1 = var_get_const(&args[0]);
    const toy_val *arg2 = var_get_const(&args[1]);
    toy_bool ret;
    if (arg1->type == VAL_MAP) {
        map_val *map = arg1->obj;
        if (arg2->type == VAL_FUNC) {
            toy_function *func = arg2->func;
            user_func_predicate predicate = { .func = func, .interp = interp };
            ret = map_not_all_match((generic_map *) map, (generic_map_filter_func) obj_val_test_predicate, &predicate);
        } else {
            invalid_argument_type(VAL_FUNC, arg2);
        }
    } else {
        invalid_argument_type(VAL_MAP, arg1);
    }
    toy_val return_val = { .type = VAL_BOOL, .boolean = ret };
    interp_set_return_value(interp, &return_val);
    return REACHED_RETURN;
}

static run_stmt_result predefined_obj_some(toy_interp *interp, const toy_var *args, size_t num_args)
{
    assert(num_args == 2);
    const toy_val *arg1 = var_get_const(&args[0]);
    const toy_val *arg2 = var_get_const(&args[1]);
    toy_bool ret;
    if (arg1->type == VAL_MAP) {
        map_val *map = arg1->obj;
        if (arg2->type == VAL_FUNC) {
            toy_function *func = arg2->func;
            user_func_predicate predicate = { .func = func, .interp = interp };
            ret = map_some_match((generic_map *) map, (generic_map_filter_func) obj_val_test_predicate, &predicate);
        } else {
            invalid_argument_type(VAL_FUNC, arg2);
        }
    } else {
        invalid_argument_type(VAL_MAP, arg1);
    }
    toy_val return_val = { .type = VAL_BOOL, .boolean = ret };
    interp_set_return_value(interp, &return_val);
    return REACHED_RETURN;
}

static run_stmt_result predefined_obj_none(toy_interp *interp, const toy_var *args, size_t num_args)
{
    assert(num_args == 2);
    const toy_val *arg1 = var_get_const(&args[0]);
    const toy_val *arg2 = var_get_const(&args[1]);
    toy_bool ret;
    if (arg1->type == VAL_MAP) {
        map_val *map = arg1->obj;
        if (arg2->type == VAL_FUNC) {
            toy_function *func = arg2->func;
            user_func_predicate predicate = { .func = func, .interp = interp };
            ret = map_none_match((generic_map *) map, (generic_map_filter_func) obj_val_test_predicate, &predicate);
        } else {
            invalid_argument_type(VAL_FUNC, arg2);
        }
    } else {
        invalid_argument_type(VAL_MAP, arg1);
    }
    toy_val return_val = { .type = VAL_BOOL, .boolean = ret };
    interp_set_return_value(interp, &return_val);
    return REACHED_RETURN;
}

double to_num(const toy_val *val)
{
    switch (val->type) {
    case VAL_BOOL:
        return val->boolean ? 0 : 1;
    case VAL_FUNC:
        invalid_argument_type(VAL_NUM, val);
        break;
    case VAL_LIST:
        return list_len((const generic_list *) val->list);
    case VAL_MAP:
        return map_size((const generic_map *) val->obj);
    case VAL_NULL:
        return 0;
    case VAL_NUM:
        return val->num;
    case VAL_STR:
        return atof(val->str);
    default:
        assert(0);
        break;
    }
    assert(0);
    return 0;
}

static run_stmt_result predefined_num(toy_interp *interp, const toy_var *args, size_t num_args)
{
    assert(num_args == 1);
    const toy_val *arg = var_get_const(&args[0]);
    toy_val return_value = { .type = VAL_NUM, .num = to_num(arg) };
    interp_set_return_value(interp, &return_value);
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
static const toy_str_list list_map_param_2 = { .str = "func", .next = NULL };
static const toy_str_list list_map_params = { .str = "list", .next = (toy_str_list *) &list_map_param_2 };
static const toy_str_list list_none_param_2 = { .str = "func", .next = NULL };
static const toy_str_list list_none_params = { .str = "list", .next = (toy_str_list *) &list_none_param_2 };
static const toy_str_list list_not_all_param_2 = { .str = "func", .next = NULL };
static const toy_str_list list_not_all_params = { .str = "list", .next = (toy_str_list *) &list_not_all_param_2 };
static const toy_str_list list_some_param_2 = { .str = "func", .next = NULL };
static const toy_str_list list_some_params = { .str = "list", .next = (toy_str_list *) &list_some_param_2 };
static const toy_str_list obj_all_param_2 = { .str = "func", .next = NULL };
static const toy_str_list obj_all_params = { .str = "map", .next = (toy_str_list *) &obj_all_param_2 };
static const toy_str_list obj_foreach_param_2 = { .str = "func", .next = NULL };
static const toy_str_list obj_foreach_params = { .str = "map", .next = (toy_str_list *) &obj_foreach_param_2 };
static const toy_str_list obj_filter_param_2 = { .str = "func", .next = NULL };
static const toy_str_list obj_filter_params = { .str = "map", .next = (toy_str_list *) &obj_filter_param_2 };
static const toy_str_list obj_keys_params = { .str = "map", .next = NULL };
static const toy_str_list obj_len_params = { .str = "map", .next = NULL };
static const toy_str_list obj_map_param_2 = { .str = "func", .next = NULL };
static const toy_str_list obj_map_params = { .str = "map", .next = (toy_str_list *) &obj_map_param_2 };
static const toy_str_list obj_none_param_2 = { .str = "func", .next = NULL };
static const toy_str_list obj_none_params = { .str = "map", .next = (toy_str_list *) &obj_none_param_2 };
static const toy_str_list obj_not_all_param_2 = { .str = "func", .next = NULL };
static const toy_str_list obj_not_all_params = { .str = "map", .next = (toy_str_list *) &obj_not_all_param_2 };
static const toy_str_list obj_some_param_2 = { .str = "func", .next = NULL };
static const toy_str_list obj_some_params = { .str = "map", .next = (toy_str_list *) &obj_some_param_2 };
static const toy_str_list num_params = { .str = "value", .next = NULL };

/* TODO: Some of these have predictable return types. Can/should we encode that in this typeless toy language? */
static const toy_function func_assert           = { .name = "assert",           .type = FUNC_PREDEFINED, .predef = predefined_assert,           .param_names = (toy_str_list *) &assert_unary_params,  .doc = "Assert that a givel value is truthy. Fail if it is not." };
static const toy_function func_assert_equal     = { .name = "assert_equal",     .type = FUNC_PREDEFINED, .predef = predefined_assert_equal,     .param_names = (toy_str_list *) &assert_binary_params, .doc = "Assert that two values are equal. Fail if they are not." };
static const toy_function func_assert_gt        = { .name = "assert_gt",        .type = FUNC_PREDEFINED, .predef = predefined_assert_gt,        .param_names = (toy_str_list *) &assert_binary_params, .doc = "Assert that the first value is strictly greater than the second. Fail if it is not." };
static const toy_function func_assert_gte       = { .name = "assert_gte",       .type = FUNC_PREDEFINED, .predef = predefined_assert_gte,       .param_names = (toy_str_list *) &assert_binary_params, .doc = "Assert that the first value is greater than or equal to the second. Fail if it is not." };
static const toy_function func_assert_lt        = { .name = "assert_lt",        .type = FUNC_PREDEFINED, .predef = predefined_assert_lt,        .param_names = (toy_str_list *) &assert_binary_params, .doc = "Assert that the first value is strictly less than the second. Fail if it is not." };
static const toy_function func_assert_lte       = { .name = "assert_lte",       .type = FUNC_PREDEFINED, .predef = predefined_assert_lte,       .param_names = (toy_str_list *) &assert_binary_params, .doc = "Assert that the first value is less than or equal to the second. Fail if it is not." };
static const toy_function func_assert_not_equal = { .name = "assert_not_equal", .type = FUNC_PREDEFINED, .predef = predefined_assert_not_equal, .param_names = (toy_str_list *) &assert_binary_params, .doc = "Assert that two values are not equal. Fail if they are." };
static const toy_function func_assert_not_null  = { .name = "assert_not_null",  .type = FUNC_PREDEFINED, .predef = predefined_assert_not_null,  .param_names = (toy_str_list *) &assert_unary_params,  .doc = "Assert that a value is not null. Fail if it is." };
static const toy_function func_assert_not_zero  = { .name = "assert_not_zero",  .type = FUNC_PREDEFINED, .predef = predefined_assert_not_zero,  .param_names = (toy_str_list *) &assert_unary_params,  .doc = "Assert that a value is not zero. Fail if it is." };
static const toy_function func_assert_null      = { .name = "assert_null",      .type = FUNC_PREDEFINED, .predef = predefined_assert_null,      .param_names = (toy_str_list *) &assert_unary_params,  .doc = "Assert that a value is null. Fail if it is not."};
static const toy_function func_assert_zero      = { .name = "assert_zero",      .type = FUNC_PREDEFINED, .predef = predefined_assert_zero,      .param_names = (toy_str_list *) &assert_unary_params,  .doc = "Assert that a value is zero. Fail if it is not." };
static const toy_function func_list_all         = { .name = "list_all",         .type = FUNC_PREDEFINED, .predef = predefined_list_all,         .param_names = (toy_str_list *) &list_all_params,      .doc = "Return true if the given function returns a truthy value when called with each item in the given list." };
static const toy_function func_list_len         = { .name = "list_len",         .type = FUNC_PREDEFINED, .predef = predefined_list_len,         .param_names = (toy_str_list *) &list_len_params,      .doc = "Count the number of items in the given list." };
static const toy_function func_list_foreach     = { .name = "list_foreach",     .type = FUNC_PREDEFINED, .predef = predefined_list_foreach,     .param_names = (toy_str_list *) &list_foreach_params,  .doc = "Call the given function once with each item of the given list." };
static const toy_function func_list_filter      = { .name = "list_filter",      .type = FUNC_PREDEFINED, .predef = predefined_list_filter,      .param_names = (toy_str_list *) &list_filter_params,   .doc = "Call the first function once with each item of the given list. If it returns a truthy value, call the second function with it." };
static const toy_function func_list_map         = { .name = "list_map",         .type = FUNC_PREDEFINED, .predef = predefined_list_map,         .param_names = (toy_str_list *) &list_map_params,      .doc = "Call the given function with each item in the given list. Return the list of return values of the function." };
static const toy_function func_list_none        = { .name = "list_none",        .type = FUNC_PREDEFINED, .predef = predefined_list_none,        .param_names = (toy_str_list *) &list_none_params,     .doc = "Return true if the given function returns a falsy value when called with each item in the given list." };
static const toy_function func_list_not_all     = { .name = "list_not_all",     .type = FUNC_PREDEFINED, .predef = predefined_list_not_all,     .param_names = (toy_str_list *) &list_not_all_params,  .doc = "Return true if the given function returns a falsy value when called with any of the items in the given list." };
static const toy_function func_list_some        = { .name = "list_some",        .type = FUNC_PREDEFINED, .predef = predefined_list_some,        .param_names = (toy_str_list *) &list_some_params,     .doc = "Return true if the given function returns a truthy value when called with any of the items in the given list." };
static const toy_function func_num              = { .name = "num",              .type = FUNC_PREDEFINED, .predef = predefined_num,              .param_names = (toy_str_list *) &num_params,           .doc = "Convert the argument to a number." };
static const toy_function func_obj_all          = { .name = "obj_all",          .type = FUNC_PREDEFINED, .predef = predefined_obj_all,          .param_names = (toy_str_list *) &obj_all_params,       .doc = "Return true if the given function returns a truthy value when called with each entry in the given object." };
static const toy_function func_obj_foreach      = { .name = "obj_foreach",      .type = FUNC_PREDEFINED, .predef = predefined_obj_foreach,      .param_names = (toy_str_list *) &obj_foreach_params,   .doc = "Call the given function once with each (name, value) entry in the given object." };
static const toy_function func_obj_filter       = { .name = "obj_filter",       .type = FUNC_PREDEFINED, .predef = predefined_obj_filter,       .param_names = (toy_str_list *) &obj_filter_params,    .doc = "Call the first function once with each (name, value) entry in the given object. If it returns a truthy value, call the second function with the same entry." };
static const toy_function func_obj_keys         = { .name = "obj_keys",         .type = FUNC_PREDEFINED, .predef = predefined_obj_keys,         .param_names = (toy_str_list *) &obj_keys_params,      .doc = "Return a list of keys in the given object." };
static const toy_function func_obj_len          = { .name = "obj_len",          .type = FUNC_PREDEFINED, .predef = predefined_obj_len,          .param_names = (toy_str_list *) &obj_len_params,       .doc = "Count the number of entries in the given object." };
static const toy_function func_obj_map          = { .name = "obj_map",          .type = FUNC_PREDEFINED, .predef = predefined_obj_map,          .param_names = (toy_str_list *) &obj_map_params,       .doc = "Call the given function with each (name, value) entry in the given object. Return an object mapping names to return values of the function." };
static const toy_function func_obj_none         = { .name = "obj_none",         .type = FUNC_PREDEFINED, .predef = predefined_obj_none,         .param_names = (toy_str_list *) &obj_none_params,      .doc = "Return true if the given function returns a falsy value when called with each entry in the given object." };
static const toy_function func_obj_not_all      = { .name = "obj_not_all",      .type = FUNC_PREDEFINED, .predef = predefined_obj_not_all,      .param_names = (toy_str_list *) &obj_not_all_params,   .doc = "Return true if the given function returns a falsy value when called with any of the entries in the given object." };
static const toy_function func_obj_some         = { .name = "obj_some",         .type = FUNC_PREDEFINED, .predef = predefined_obj_some,         .param_names = (toy_str_list *) &obj_some_params,      .doc = "Return true if the given function returns a truthy value when called with any of the entries in the given object." };
static const toy_function func_print            = { .name = "print",            .type = FUNC_PREDEFINED, .predef = predefined_print,            .param_names = (toy_str_list *) &INFINITE_PARAMS,      .doc = "Output the given message to the console." };

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
    { .type = VAL_FUNC, .func = (toy_function *) &func_list_filter },
    { .type = VAL_FUNC, .func = (toy_function *) &func_list_foreach },
    { .type = VAL_FUNC, .func = (toy_function *) &func_list_map },
    { .type = VAL_FUNC, .func = (toy_function *) &func_list_none },
    { .type = VAL_FUNC, .func = (toy_function *) &func_list_not_all },
    { .type = VAL_FUNC, .func = (toy_function *) &func_list_some },
    { .type = VAL_FUNC, .func = (toy_function *) &func_num },
    { .type = VAL_FUNC, .func = (toy_function *) &func_obj_all },
    { .type = VAL_FUNC, .func = (toy_function *) &func_obj_filter },
    { .type = VAL_FUNC, .func = (toy_function *) &func_obj_foreach },
    { .type = VAL_FUNC, .func = (toy_function *) &func_obj_keys },
    { .type = VAL_FUNC, .func = (toy_function *) &func_obj_len },
    { .type = VAL_FUNC, .func = (toy_function *) &func_obj_map },
    { .type = VAL_FUNC, .func = (toy_function *) &func_obj_none },
    { .type = VAL_FUNC, .func = (toy_function *) &func_obj_not_all },
    { .type = VAL_FUNC, .func = (toy_function *) &func_obj_some },
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
