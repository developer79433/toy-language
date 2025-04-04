#include <stdio.h>

#include "errors.h"
#include "dump.h"
#include "list.h"

void fatal_error(const char *fmt, ...)
{
    va_list argptr;
    va_start(argptr, fmt);
    vfprintf(stderr, fmt, argptr);
    va_end(argptr);
    fputc('\n', stderr);
    exit(1);
}

/* TODO: Maybe "invalid argument" makes more sense? */

void invalid_operand(enum toy_expr_type expr_type, const toy_expr *operand)
{
    dump_expr(stderr, operand);
    fputc('\n', stderr);
    fatal_error("Invalid operand for %s", toy_expr_type_name(expr_type));
}

void invalid_operands(enum toy_expr_type expr_type, const toy_expr *operand1, const toy_expr *operand2)
{
    dump_expr(stderr, operand1);
    fputc('\n', stderr);
    dump_expr(stderr, operand2);
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
void invalid_cast(enum toy_expr_type expr_type, const toy_expr *expr)
{
    dump_expr(stderr, expr);
    fputc('\n', stderr);
    fatal_error("Cannot convert to %s", toy_expr_type_name(expr_type));
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

void invalid_list_index(toy_list *list, toy_num index)
{
    dump_list(stderr, list);
    fputc('\n', stderr);
    fatal_error("Invalid list index %d", index);
}

void too_few_arguments(toy_num expected, toy_list *args)
{
    dump_list(stderr, args);
    fputc('\n', stderr);
    fatal_error("Too few arguments: expected %d, received %d", expected, list_len(args));
}

void too_many_arguments(toy_num expected, toy_list *args)
{
    dump_list(stderr, args);
    fputc('\n', stderr);
    fatal_error("Too many arguments: expected %d, received %d", expected, list_len(args));
}

void invalid_string_index(toy_str str, toy_num index)
{
    dump_str(stderr, str);
    fputc('\n', stderr);
    fatal_error("Invalid string index %d", index);
}

void invalid_argument_type(enum toy_val_type expected_type, const toy_val *actual_arg)
{
    dump_literal(stderr, actual_arg);
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
