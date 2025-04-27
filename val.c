#include <stdio.h>
#include <assert.h>

#include "mymalloc.h"
#include "bool.h"
#include "str.h"
#include "val.h"
#include "function.h"
#include "val-list.h"
#include "map.h"
#include "errors.h"
#include "constants.h"

static const char *toy_val_type_names[] = {
    "boolean",
    "function",
    "list",
    "map",
    "null",
    "numeric",
    "string"
};

const char *toy_val_type_name(enum toy_val_type val_type)
{
    return toy_val_type_names[val_type];
}

void dump_val(FILE *f, const toy_val *val)
{
    if (val) {
        switch(val->type) {
        case VAL_BOOL:
            dump_bool(f, val->boolean);
            break;
        case VAL_FUNC:
            func_dump(f, val->func);
            break;
        case VAL_LIST:
            val_list_dump(f, val->list);
            break;
        case VAL_MAP:
            map_dump(f, val->map);
            break;
        case VAL_NULL:
            fputs("null", f);
            break;
        case VAL_NUM:
            fprintf(f, "%f", val->num);
            break;
        case VAL_STR:
            dump_str(f, val->str);
            break;
        default:
            invalid_value_type(val->type);
            break;
        }
    } else {
        dump_val(f, &null_val);
    }
}

toy_bool toy_vals_equal(const toy_val *val1, const toy_val *val2)
{
    if (val1->type == val2->type) {
        switch (val1->type) {
        case VAL_BOOL:
            return (val1->boolean == val2->boolean);
        case VAL_FUNC:
            return (val1->func == val2->func);
            break;
        case VAL_LIST:
            return (val1->list == val2->list);
            break;
        case VAL_MAP:
            return (val1->map == val2->map);
            break;
        case VAL_NULL:
            return TOY_TRUE;
            break;
        case VAL_NUM:
            return (val1->num == val2->num);
            break;
        case VAL_STR:
            return toy_str_equal(val1->str, val2->str);
            break;
        default:
            invalid_operands(EXPR_EQUAL, val1, val2);
            break;
        }
    }
    return TOY_FALSE;
}

toy_bool toy_vals_nequal(const toy_val *val1, const toy_val *val2)
{
    return !toy_vals_equal(val1, val2);
}

toy_bool toy_val_gt(const toy_val *val1, const toy_val *val2)
{
    assert(val1->type == VAL_NUM);
    assert(val2->type == VAL_NUM);
    return (val1->num > val2->num);
}

toy_bool toy_val_gte(const toy_val *val1, const toy_val *val2)
{
    assert(val1->type == VAL_NUM);
    assert(val2->type == VAL_NUM);
    return (val1->num >= val2->num);
}

toy_bool toy_val_lt(const toy_val *val1, const toy_val *val2)
{
    assert(val1->type == VAL_NUM);
    assert(val2->type == VAL_NUM);
    return (val1->num < val2->num);
}

toy_bool toy_val_lte(const toy_val *val1, const toy_val *val2)
{
    assert(val1->type == VAL_NUM);
    assert(val2->type == VAL_NUM);
    return (val1->num <= val2->num);
}

void val_assert_valid(toy_val *val)
{
    switch(val->type) {
    case VAL_BOOL:
        bool_assert_valid(val->boolean);
        break;
    case VAL_FUNC:
        func_assert_valid(val->func);
        break;
    case VAL_LIST:
        val_list_assert_valid(val->list);
        break;
    case VAL_MAP:
        map_assert_valid(val->map);
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
