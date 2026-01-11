#include <assert.h>
#include <string.h>

#include "mymalloc.h"
#include "bool.h"
#include "str.h"
#include "val.h"
#include "function.h"
#include "val-list.h"
#include "generic-map.h"
#include "errors.h"
#include "constants.h"
#include "map-val.h"
#include "debug.h"
#include "log.h"

static const char *toy_val_type_names[] = {
    "boolean",
    "function",
    "list",
    "map",
    "null",
    "numeric",
    "string"
};

const char *val_type_name(toy_val_type val_type)
{
    return toy_val_type_names[val_type];
}

void val_dump(const toy_val *val, toy_bool verbose)
{
    if (val) {
        switch(val->type) {
        case VAL_BOOL:
            dump_bool(val->boolean);
            break;
        case VAL_FUNC:
            func_dump(val->func, verbose);
            break;
        case VAL_LIST:
            val_list_dump(val->list);
            break;
        case VAL_MAP:
            map_val_dump(val->map);
            break;
        case VAL_NULL:
            log_puts("null");
            break;
        case VAL_NUM:
            log_printf("%f", val->num);
            break;
        case VAL_STR:
            str_dump(val->str);
            break;
        default:
            invalid_value_type(val->type);
            break;
        }
    } else {
        val_dump(&null_val, verbose);
    }
}

toy_bool val_truthy(const toy_val *val)
{
    switch(val->type) {
    case VAL_BOOL:
        return val->boolean == TOY_TRUE;
    case VAL_FUNC:
        return TOY_TRUE;
    case VAL_LIST:
        return val_list_len(val->list) != 0;
    case VAL_MAP:
        return map_val_size(val->map) != 0;
    case VAL_NULL:
        return TOY_FALSE;
    case VAL_NUM:
        return val->num != 0;
    case VAL_STR:
        return (val->str != NULL && strlen(val->str) != 0);
    default:
        invalid_value_type(val->type);
        break;
    }
    assert(0);
    return TOY_FALSE;
}

toy_bool val_falsey(const toy_val *val)
{
    return !val_truthy(val);
}

toy_bool val_list_equal(const toy_val_list *list1, const toy_val_list *list2)
{
    /* TODO: Use a joint enumeration function yet to be created */
    for (; list1 && list2 ; list1 = list1->next, list2 = list2->next) {
        const toy_val *val1 = val_list_payload_const(list1);
        const toy_val *val2 = val_list_payload_const(list2);
        if (vals_nequal(val1, val2)) {
            return TOY_FALSE;
        }
    }
    return TOY_TRUE;
}

toy_bool vals_equal(const toy_val *val1, const toy_val *val2)
{
    if (val1->type == val2->type) {
        switch (val1->type) {
        case VAL_BOOL:
            return (val1->boolean == val2->boolean);
        case VAL_FUNC:
            return (val1->func == val2->func);
            break;
        case VAL_LIST:
            return val_list_equal(val1->list, val2->list);
            break;
        case VAL_MAP:
            return map_val_equal(val1->map, val2->map);
            break;
        case VAL_NULL:
            return TOY_TRUE;
            break;
        case VAL_NUM:
            return (val1->num == val2->num);
            break;
        case VAL_STR:
            return str_equal(val1->str, val2->str);
            break;
        default:
            invalid_operands(EXPR_EQUAL, val1, val2);
            break;
        }
    }
    return TOY_FALSE;
}

toy_bool vals_nequal(const toy_val *val1, const toy_val *val2)
{
    return !vals_equal(val1, val2);
}

toy_bool num_gt(toy_num num1, toy_num num2)
{
    return (num1 > num2);
}

toy_bool num_gte(toy_num num1, toy_num num2)
{
    return (num1 >= num2);
}

toy_bool num_lt(toy_num num1, toy_num num2)
{
    return (num1 < num2);
}

toy_bool num_lte(toy_num num1, toy_num num2)
{
    return (num1 <= num2);
}

toy_bool val_gt(const toy_val *val1, const toy_val *val2)
{
    assert(val1->type == VAL_NUM);
    assert(val2->type == VAL_NUM);
    return num_gt(val1->num, val2->num);
}

toy_bool val_gte(const toy_val *val1, const toy_val *val2)
{
    assert(val1->type == VAL_NUM);
    assert(val2->type == VAL_NUM);
    return num_gte(val1->num, val2->num);
}

toy_bool val_lt(const toy_val *val1, const toy_val *val2)
{
    assert(val1->type == VAL_NUM);
    assert(val2->type == VAL_NUM);
    return num_lt(val1->num, val2->num);
}

toy_bool val_lte(const toy_val *val1, const toy_val *val2)
{
    assert(val1->type == VAL_NUM);
    assert(val2->type == VAL_NUM);
    return num_lte(val1->num, val2->num);
}

#ifndef NDEBUG

void val_type_assert_valid(toy_val_type val_type)
{
    assert(val_type >= 0);
    assert(val_type <= VAL_MAX);
    switch (val_type) {
    case VAL_BOOL:
    case VAL_FUNC:
    case VAL_LIST:
    case VAL_MAP:
    case VAL_NULL:
    case VAL_NUM:
    case VAL_STR:
        break;
    default:
        invalid_value_type(val_type);
        break;
    }
}

void val_assert_valid(const toy_val *val)
{
    assert(val != NULL);
    val_type_assert_valid(val->type);
    switch (val->type) {
    case VAL_BOOL:
        bool_assert_valid(val->boolean);
        break;
    case VAL_FUNC:
        if (valid_check_depth < VALID_CHECK_RECURSION_DEPTH) {
            valid_check_depth++;
            func_assert_valid(val->func);
            valid_check_depth--;
        }
        break;
    case VAL_LIST:
        if (valid_check_depth < VALID_CHECK_RECURSION_DEPTH) {
            valid_check_depth++;
            val_list_assert_valid(val->list);
            valid_check_depth--;
        }
        break;
    case VAL_MAP:
        if (valid_check_depth < VALID_CHECK_RECURSION_DEPTH) {
            valid_check_depth++;
            map_val_assert_valid(val->map);
            valid_check_depth--;
        }
        break;
    case VAL_NULL:
        break;
    case VAL_NUM:
        break;
    case VAL_STR:
        str_assert_valid(val->str);
        break;
    default:
        invalid_value_type(val->type);
        break;
    }
}

void val_assert_valid_or_null(const toy_val *val)
{
    if (val != NULL) {
        val_assert_valid(val);
    }
}

#endif /* NDEBUG */

void val_free(toy_val *val)
{
    val_assert_valid(val);
    switch (val->type) {
    case VAL_BOOL:
        break;
    case VAL_FUNC:
        func_free(val->func);
        break;
    case VAL_LIST:
        val_list_free(val->list);
        break;
    case VAL_MAP:
        map_val_free(val->map);
        break;
    case VAL_NULL:
        break;
    case VAL_NUM:
        break;
    case VAL_STR:
        str_free(val->str);
        break;
    default:
        invalid_value_type(val->type);
        break;
    }
    free(val);
}

void assert_vals_equal(const toy_val *val1, const toy_val *val2)
{
    val_assert_valid(val1);
    val_assert_valid(val2);
    toy_bool compare_res = vals_equal(val1, val2);
    if (compare_res == TOY_TRUE) {
        return;
    }
    val_dump(val1, 1);
    val_dump(val2, 1);
    assert(0);
}

toy_val *val_alloc_func_decl(toy_str_list *formalparams, toy_block *body)
{
    toy_val *val;
    val = (toy_val *) malloc(sizeof(toy_val) + sizeof(toy_function));
    val->type = VAL_FUNC;
    val->func = (toy_function *) (val + 1);
    val->func->type = FUNC_USER_DECLARED;
    val->func->name = ""; /* TODO: generated unique name */
    val->func->param_names = formalparams;
    val->func->code->stmts = body->stmts;
    return val;
}

void val_array_dump(const toy_val *vals, size_t size)
{
    toy_bool printed_anything = TOY_FALSE;
    for (const toy_val *val = vals; val < &vals[size]; val++) {
        if (printed_anything) {
            log_puts(", ");
        }
        val_dump(val, 1);
        printed_anything = TOY_TRUE;
    }
}
