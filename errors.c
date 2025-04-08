#include <stdio.h>

#include "errors.h"
#include "dump.h"
#include "toy-val-list.h"
#include "toy-expr-list.h"
#include "toy-str.h"
#include "toy-val.h"
#include "functions.h"

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

void invalid_operand(enum toy_expr_type expr_type, const toy_val *operand)
{
    dump_val(stderr, operand);
    fputc('\n', stderr);
    fatal_error("Invalid operand for %s", toy_expr_type_name(expr_type));
}

void invalid_operands(enum toy_expr_type expr_type, const toy_val *operand1, const toy_val *operand2)
{
    dump_val(stderr, operand1);
    fputc('\n', stderr);
    dump_val(stderr, operand2);
    fputc('\n', stderr);
    fatal_error("Invalid operands for %s", toy_expr_type_name(expr_type));
}

void invalid_expr_type(enum toy_expr_type expr_type) {
    fatal_error("Invalid expression type %s", toy_expr_type_name(expr_type));
}

void invalid_stmt_type(enum toy_stmt_type stmt_type)
{
    fatal_error("Invalid statement type %s", toy_stmt_type_name(stmt_type));
}

/* TODO: Delete me */
void invalid_cast(enum toy_val_type val_type, const toy_val *val)
{
    dump_val(stderr, val);
    fputc('\n', stderr);
    fatal_error("Cannot convert to %s", toy_val_type_name(val_type));
}

void duplicate_identifier(toy_str name)
{
    fatal_error("Attempt to redeclare identifier '%s'", name);
}

void undeclared_identifier(toy_str name)
{
    fatal_error("Undeclared identifier '%s'", name);
}

void readonly_identifier(toy_str name)
{
    fatal_error("Read-only identifier '%s'", name);
}

void invalid_val_list_index(toy_val_list *list, toy_num index)
{
    dump_val_list(stderr, list);
    fputc('\n', stderr);
    fatal_error("Invalid val list index %d", index);
}

void too_few_arguments(toy_num expected, toy_expr_list *args)
{
    dump_expr_list(stderr, args);
    fputc('\n', stderr);
    fatal_error("Too few arguments: expected %d, received %d", expected, expr_list_len(args));
}

void too_many_arguments(toy_num expected, toy_expr_list *args)
{
    dump_expr_list(stderr, args);
    fputc('\n', stderr);
    fatal_error("Too many arguments: expected %d, received %d", expected, expr_list_len(args));
}

void invalid_string_index(toy_str str, toy_num index)
{
    dump_str(stderr, str);
    fputc('\n', stderr);
    fatal_error("Invalid string index %d", index);
}

void invalid_argument_type(enum toy_val_type expected_type, const toy_val *actual_arg)
{
    dump_val(stderr, actual_arg);
    fputc('\n', stderr);
    fatal_error("Invalid argument type: expected %s", toy_val_type_name(expected_type));
}

void invalid_value_type(enum toy_val_type value_type)
{
    fatal_error("Invalid value type %s", toy_val_type_name(value_type));
}

void divide_by_zero(void)
{
    fatal_error("Divide by zero");
}

const char *func_type_names[] = {
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

void return_outside_function(enum block_type btype)
{
    fatal_error("Encountered return statement outside function, in block of type %s", block_type_name(btype));
}
