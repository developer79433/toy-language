#ifndef TOY_ERRORS_H
#define TOY_ERRORS_H 1

#include <stdarg.h>

#include "expr-types.h"
#include "stmt-types.h"
#include "interp-types.h"
#include "function-types.h"

void fatal_error(const char *fmt, ...);
void undeclared_identifier(const toy_str name);
void duplicate_identifier(const toy_str name);
void readonly_identifier(const toy_str name);
void invalid_operand(toy_expr_type expr_type, const toy_val *operand);
void invalid_operands(toy_expr_type expr_type, const toy_val *operand1, const toy_val *operand2);
void invalid_expr_type(toy_expr_type expr_type);
void invalid_stmt_type(toy_stmt_type stmt_type);
void invalid_cast(toy_val_type val_type, const toy_val *val);
void invalid_val_list_index(const toy_val_list *list, toy_num index);
void invalid_string_index(const toy_str str, toy_num index);
void too_few_arguments(toy_num expected, const toy_expr_list *args);
void too_many_arguments(toy_num expected, const toy_expr_list *args);
void invalid_argument_type(toy_val_type expected_type, const toy_val *actual_arg);
void invalid_value_type(toy_val_type value_type);
void divide_by_zero(void);
void invalid_function_type(enum toy_func_type func_type);
void return_outside_function(frame_type ftype);

#endif /* TOY_ERRORS_H */
