#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "errors.h"
#include "dump.h"
#include "val-list.h"
#include "expr-list.h"
#include "str.h"
#include "val.h"
#include "expr.h"
#include "stmt.h"
#include "function.h"
#include "interp.h"
#include "interp-frame.h"

void fatal_error(const char *fmt, ...)
{
    va_list argptr;
    va_start(argptr, fmt);
    vfprintf(stderr, fmt, argptr);
    va_end(argptr);
    fputc('\n', stderr);
    abort();
}

/* TODO: Maybe "invalid argument" makes more sense? */

void invalid_operand(toy_expr_type expr_type, const toy_val *operand)
{
    val_dump(stderr, operand);
    fputc('\n', stderr);
    fatal_error("Invalid operand for %s", toy_expr_type_name(expr_type));
}

void invalid_operands(toy_expr_type expr_type, const toy_val *operand1, const toy_val *operand2)
{
    val_dump(stderr, operand1);
    fputc('\n', stderr);
    val_dump(stderr, operand2);
    fputc('\n', stderr);
    fatal_error("Invalid operands for %s", toy_expr_type_name(expr_type));
}

void invalid_expr_type(toy_expr_type expr_type) {
    fatal_error("Invalid expression type %s", toy_expr_type_name(expr_type));
}

void invalid_stmt_type(toy_stmt_type stmt_type)
{
    fatal_error("Invalid statement type %s", stmt_type_name(stmt_type));
}

/* TODO: Delete me */
void invalid_cast(toy_val_type val_type, const toy_val *val)
{
    val_dump(stderr, val);
    fputc('\n', stderr);
    fatal_error("Cannot convert to %s", val_type_name(val_type));
}

void duplicate_identifier(const toy_str name)
{
    fatal_error("Attempt to redeclare identifier '%s'", name);
}

void undeclared_identifier(const toy_str name)
{
    fatal_error("Undeclared identifier '%s'", name);
}

void readonly_identifier(const toy_str name)
{
    fatal_error("Read-only identifier '%s'", name);
}

void invalid_val_list_index(const toy_val_list *list, toy_num index)
{
    val_list_dump(stderr, list);
    fputc('\n', stderr);
    fatal_error("Invalid val list index %d", index);
}

void too_few_arguments(toy_num expected, const toy_expr_list *args)
{
    expr_list_dump(stderr, args);
    fputc('\n', stderr);
    fatal_error("Too few arguments: expected %d, received %d", expected, expr_list_len(args));
}

void too_many_arguments(toy_num expected, const toy_expr_list *args)
{
    expr_list_dump(stderr, args);
    fputc('\n', stderr);
    fatal_error("Too many arguments: expected %d, received %d", expected, expr_list_len(args));
}

void invalid_string_index(const toy_str str, toy_num index)
{
    dump_str(stderr, str);
    fputc('\n', stderr);
    fatal_error("Invalid string index %d", index);
}

void invalid_argument_type(toy_val_type expected_type, const toy_val *actual_arg)
{
    val_dump(stderr, actual_arg);
    fputc('\n', stderr);
    fatal_error("Invalid argument type: expected %s", val_type_name(expected_type));
}

void invalid_value_type(toy_val_type value_type)
{
    fatal_error("Invalid value type %s", val_type_name(value_type));
}

void divide_by_zero(void)
{
    fatal_error("Divide by zero");
}

static const char *func_type_names[] = {
    "Predefined",
    "User-defined"
};

/* TODO: Belongs elsewhere */
static const char *function_type_name(enum toy_func_type func_type)
{
    return func_type_names[func_type];
}

void invalid_function_type(enum toy_func_type func_type)
{
    fatal_error("Invalid function type %s (%d)", function_type_name(func_type), (int) func_type);
}

void return_outside_function(frame_type ftype)
{
    fatal_error("Encountered return statement outside function, in frame of type %s", interp_frame_type_name(ftype));
}

/* TODO: Belongs elsewhere */

static const char *reference_type_names[] = {
    "undefined reference",
    "function declaration",
    "function parameter",
    "local variable",
    "predefined constant",
    "predefined function"
};

const char *resolved_name_type_name(reference_type reftype)
{
    return reference_type_names[(int) reftype];
}

const char *resolved_name_ref_name(const resolved_name *resolved)
{
    switch (resolved->type) {
    case REF_UNDEFINED:
        return "null";
    case REF_FUNC_DECL:
        toy_func_decl_stmt *func_decl = resolved->func_decl;
        toy_function *func = &func_decl->func;
        return func->name;
    case REF_FUNC_PARAM:
        const func_param_ref *param_ref = &resolved->func_param;
        assert(param_ref);
        /* TODO */
        return "function parameter";
    case REF_VAR_DECL:
        const toy_var_decl *var_decl = resolved->var_decl;
        return var_decl->name;
    case REF_PREDEF_CONST:
        const toy_val *predef_const = resolved->predef_const;
        assert(predef_const);
        /* TODO */
        return "predefined constant";
    case REF_PREDEF_FUNC:
        const toy_val *predef_func_val = resolved->predef_func;
        assert(VAL_FUNC == predef_func_val->type);
        const toy_function *predef_func = predef_func_val->func;
        return predef_func->name;
    default:
        assert(0);
        break;
    }
}

void invalid_lvalue(resolved_name *resolved)
{
    fatal_error("Cannot assign to %s", resolved_name_type_name(resolved->type), resolved_name_ref_name(resolved));
}
