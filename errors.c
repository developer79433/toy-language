#include <stdlib.h>
#include <assert.h>

#include "errors.h"
#include "val-list.h"
#include "expr-list.h"
#include "str.h"
#include "val.h"
#include "expr.h"
#include "stmt.h"
#include "function.h"
#include "interp.h"
#include "interp-frame.h"
#include "log.h"
#include "str-list.h"
#include "var-decl.h"
#include "decl-ref.h"

void fatal_error(const toy_str fmt, ...)
{
    va_list argptr;
    va_start(argptr, fmt);
    log_vprintf(LOG_ERROR, fmt, argptr);
    va_end(argptr);
    log_putc(LOG_ERROR, L'\n');
    abort();
}

/* TODO: Maybe "invalid argument" makes more sense? */

void invalid_operand(toy_expr_type expr_type, const toy_val *operand)
{
    log_debug(L"Operand was:\n");
    val_dump(operand, 1);
    log_putc(LOG_DEBUG, L'\n');
    fatal_error(L"Invalid operand for %ls", toy_expr_type_name(expr_type));
}

void invalid_operands(toy_expr_type expr_type, const toy_val *operand1, const toy_val *operand2)
{
    log_debug(L"Operands were:\n");
    val_dump(operand1, 1);
    log_putc(LOG_DEBUG, L'\n');
    val_dump(operand2, 1);
    log_putc(LOG_DEBUG, L'\n');
    fatal_error(L"Invalid operands for %ls", toy_expr_type_name(expr_type));
}

void invalid_expr_type(toy_expr_type expr_type) {
    fatal_error(L"Invalid expression type %ls", toy_expr_type_name(expr_type));
}

void invalid_stmt_type(toy_stmt_type stmt_type)
{
    fatal_error(L"Invalid statement type %d", stmt_type);
}

/* TODO: Delete me */
void invalid_cast(toy_val_type val_type, const toy_val *val)
{
    val_dump(val, 1);
    log_putc(LOG_DEBUG, L'\n');
    fatal_error(L"Cannot convert to %ls", val_type_name(val_type));
}

void duplicate_identifier(const toy_str name)
{
    fatal_error(L"Attempt to redeclare identifier '%ls'", name);
}

void undeclared_identifier(const toy_str name)
{
    fatal_error(L"Undeclared identifier '%ls'", name);
}

void readonly_identifier(const toy_str name)
{
    fatal_error(L"Read-only identifier '%ls'", name);
}

void invalid_val_list_index(const toy_val_list *list, toy_num index)
{
    val_list_dump(list);
    log_putc(LOG_DEBUG, L'\n');
    fatal_error(L"Invalid val list index %d", index);
}

void too_few_arguments(toy_num expected, const toy_expr_list *args)
{
    expr_list_dump(args, TOY_TRUE);
    log_putc(LOG_DEBUG, L'\n');
    fatal_error(L"Too few arguments: expected %d, received %d", expected, expr_list_len(args));
}

void too_many_arguments(toy_num expected, const toy_expr_list *args)
{
    expr_list_dump(args, TOY_TRUE);
    log_putc(LOG_DEBUG, L'\n');
    fatal_error(L"Too many arguments: expected %d, received %d", expected, expr_list_len(args));
}

void invalid_string_index(const toy_str str, toy_num index)
{
    str_dump(str, TOY_TRUE);
    log_putc(LOG_DEBUG, L'\n');
    fatal_error(L"Invalid string index %d", index);
}

void invalid_argument_type(toy_val_type expected_type, const toy_val *actual_arg)
{
    val_dump(actual_arg, 1);
    log_putc(LOG_DEBUG, L'\n');
    fatal_error(L"Invalid argument type: expected %ls", val_type_name(expected_type));
}

void invalid_value_type(toy_val_type value_type)
{
    fatal_error(L"Invalid value type %d", value_type);
}

void divide_by_zero(void)
{
    fatal_error(L"Divide by zero");
}

void return_outside_function(frame_type ftype)
{
    fatal_error(L"Encountered return statement outside function, in frame of type %ls", interp_frame_type_name(ftype));
}

void invalid_lvalue(decl_ref *ref)
{
    fatal_error(L"Cannot assign to %ls", decl_ref_type_name(ref->type));
}

void illegal_instruction_in_for_stmt_at_start(const toy_stmt *stmt)
{
    fatal_error(L"Illegal statement type %ls in for statement loop start", stmt_type_name(stmt->type));
}

void illegal_instruction_in_for_stmt_at_end(const toy_stmt *stmt)
{
    fatal_error(L"Illegal statement type %ls in for statement loop end", stmt_type_name(stmt->type));
}

void no_return_value(const toy_function *func)
{
    fatal_error(L"Function %ls did not return a value", func->name);
}

void incorrect_function_num_args(toy_function *func, size_t required_num_args)
{
    fatal_error(L"Function %ls must take %zu arguments, but actually takes %zu", func->name, required_num_args, str_list_len(func->param_names));
}
