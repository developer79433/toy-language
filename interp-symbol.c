#include <assert.h>

#include "interp-symbol.h"
#include "str.h"
#include "interp.h"
#include "interp-frame.h"
#include "constants.h"
#include "predef-function.h"
#include "map-val.h"
#include "val.h"
#include "expr.h"
#include "errors.h"

static get_result lookup_user_identifier(toy_interp *interp, toy_val *result, toy_str name)
{
    assert(!lookup_predefined_constant(name));
    assert(!predef_func_lookup_name(name));
    /* TODO: Use interp_frame_foreach */
    for (interp_frame_list *frame = interp_cur_frame(interp); frame; frame = frame->prev) {
        get_result found = lookup_identifier_in_frame(frame, result, name);
        if (found == GET_FOUND) {
            return GET_FOUND;
        }
    }
    return GET_NOT_FOUND;
}

get_result lookup_identifier(toy_interp *interp, toy_val *result, const toy_str name)
{
    const toy_val *predef_const = lookup_predefined_constant(name);
    if (predef_const) {
        *result = *((toy_val *) predef_const);
        return GET_FOUND;
    }
    const toy_function *predef_func = predef_func_lookup_name(name);
    if (predef_func) {
        result->type = VAL_FUNC;
        result->func = (toy_function *) predef_func;
        return GET_FOUND;
    }
    return lookup_user_identifier(interp, result, name);
}

enum set_variable_policy_enum {
    POLICY_MUST_ALREADY_EXIST,
    POLICY_MUST_NOT_ALREADY_EXIST
};
typedef enum set_variable_policy_enum set_variable_policy;

static set_result interp_set_symbol(toy_interp *interp, const toy_str name, const toy_val *value)
{
    interp_frame_list *frame_list = interp_cur_frame(interp);
    interp_frame *cur_frame = interp_frame_list_payload(frame_list);
    if (cur_frame->symbols == NULL) {
        cur_frame->symbols = map_val_alloc();
    }
    set_result res = map_val_set(cur_frame->symbols, name, (toy_val *) value);
    return res;
}

static set_result set_variable_value_policy(toy_interp *interp, const toy_str name, const toy_val *value, set_variable_policy policy)
{
    str_assert_valid(name);
    val_assert_valid(value);
#if DEBUG_VARIABLES
    fprintf(stderr, "Set %s variable '%s' to ", ((POLICY_MUST_ALREADY_EXIST == policy) ? "existing" : "new"), name);
    val_dump(stderr, value);
    fputc('\n', stderr);
#endif /* DEBUG_VARIABLES */
    if (is_predefined(name)) {
        readonly_identifier(name);
    }
    toy_val old_value;
    get_result get_res = lookup_identifier(interp, &old_value, name);
    switch (policy) {
    case POLICY_MUST_ALREADY_EXIST:
        if (get_res == GET_NOT_FOUND) {
            undeclared_identifier(name);
        }
        break;
    case POLICY_MUST_NOT_ALREADY_EXIST:
        if (get_res == GET_FOUND) {
            duplicate_identifier(name);
        }
        break;
    default:
        assert(0);
        break;
    }
    set_result set_res = interp_set_symbol(interp, name, value);
    assert(
        (set_res == SET_NEW && policy == POLICY_MUST_NOT_ALREADY_EXIST)
        ||
        (set_res == SET_EXISTING && policy == POLICY_MUST_ALREADY_EXIST)
    );
    return set_res;
}

void set_variable_value(toy_interp *interp, const toy_str name, const toy_val *value)
{
    val_assert_valid(value);
    set_result res = set_variable_value_policy(interp, name, value, POLICY_MUST_ALREADY_EXIST);
    assert(res == SET_EXISTING);
}

void create_variable_value(toy_interp *interp, const toy_str name, const toy_val *value)
{
    val_assert_valid(value);
    set_result res = set_variable_value_policy(interp, name, value, POLICY_MUST_NOT_ALREADY_EXIST);
    assert(res == SET_NEW);
}

void create_variable_expr(toy_interp *interp, const toy_str name, const toy_expr *expr)
{
    toy_val value;
    expr_assert_valid(expr);
    expr_eval(interp, &value, expr);
    val_assert_valid(&value);
    create_variable_value(interp, name, &value);
}

void create_function(toy_interp *interp, const toy_function *def)
{
    if (is_predefined(def->name)) {
        readonly_identifier(def->name);
    }
    toy_val old_value;
    get_result get_res = lookup_identifier(interp, &old_value, def->name);
    if (get_res == GET_FOUND) {
        duplicate_identifier(def->name);
    }
    const toy_val func_val = { .type = VAL_FUNC, .func = (toy_function *) def };
    set_result set_res = interp_set_symbol(interp, def->name, &func_val);
    assert(set_res == SET_NEW);
}

void op_assign(toy_interp *interp, toy_val *result, toy_str name, toy_expr *value)
{
    toy_val value_result;
    expr_eval(interp, &value_result, value);
    set_variable_value(interp, name, &value_result);
    *result = value_result;
}
