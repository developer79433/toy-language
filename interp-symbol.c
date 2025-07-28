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
#include "function.h"
#include "log.h"
#include "symbol-table.h"
#include "interp-stack.h"

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
    interp_stack *stack = interp_get_stack(interp);
    return lookup_user_identifier(stack, result, name);
}

enum set_variable_policy_enum {
    POLICY_MUST_ALREADY_EXIST,
    POLICY_MUST_NOT_ALREADY_EXIST
};
typedef enum set_variable_policy_enum set_variable_policy;

static set_result interp_set_symbol(toy_interp *interp, const toy_str name, const toy_val *value)
{
    interp_frame *frame = interp_cur_frame(interp);
    return symbol_table_set(&frame->symbols, name, value);
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

void set_variable_value(toy_interp *interp, toy_str name, toy_val *value)
{
    val_assert_valid(value);
    /* TODO */
#if 0
    switch (resolved->type) {
    case REF_FUNC_DECL:
        resolved->func_decl;
        break;
    case REF_FUNC_PARAM:
        resolved->func_param_index;
        break;
    case REF_PREDEF_CONST:
        resolved->predef_const;
        break;
    case REF_PREDEF_FUNC:
        resolved->predef_func;
        break;
    case REF_UNDEFINED:
        assert(0);
        break;
    case REF_VAR_DECL:
        toy_var_decl *var_decl = resolved->var_decl;
        var_decl->value;
        break;
    default:
        assert(0);
        break;
    }
    ref->val = value;
#endif
}

void create_variable_value(toy_interp *interp, const toy_str name, const toy_val *value)
{
    val_assert_valid(value);
    set_result res = set_variable_value_policy(interp, name, value, POLICY_MUST_NOT_ALREADY_EXIST);
    assert(res == SET_NEW);
}

void create_variable_expr(toy_interp *interp, const toy_str name, toy_expr *expr)
{
    toy_val value;
    expr_assert_valid(expr);
    expr_eval(interp, &value, expr);
    val_assert_valid(&value);
    create_variable_value(interp, name, &value);
}

void create_function(toy_interp *interp, toy_function *func)
{
    if (is_predefined(func->name)) {
        readonly_identifier(func->name);
    }
    toy_val old_value;
    get_result get_res = lookup_identifier(interp, &old_value, func->name);
    if (get_res == GET_FOUND) {
        duplicate_identifier(func->name);
    }
    const toy_val func_val = { .type = VAL_FUNC, .func = (toy_function *) func };
    func_assert_valid(func);
    val_assert_valid(&func_val);
    set_result set_res = interp_set_symbol(interp, func->name, &func_val);
    assert(set_res == SET_NEW);
}

void op_assign(toy_interp *interp, toy_val *result, resolved_name *lhs, toy_expr *rhs)
{
    toy_val rhs_result;
    expr_eval(interp, &rhs_result, rhs);
    toy_val *val = get_lvalue(interp, lhs);
    *result = *val = rhs_result;
}
