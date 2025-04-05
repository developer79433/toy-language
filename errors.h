#ifndef TOY_ERRORS_H
#define TOY_ERRORS_H 1

#include <stdarg.h>

#include "ast.h"

void fatal_error(const char *fmt, ...);
void undeclared_identifier(toy_str name);
void duplicate_identifier(toy_str name);
void readonly_identifier(toy_str name);
void invalid_operand(enum toy_expr_type expr_type, const toy_val *operand);
void invalid_operands(enum toy_expr_type expr_type, const toy_val *operand1, const toy_val *operand2);
void invalid_expr_type(enum toy_expr_type expr_type);
void invalid_stmt_type(enum toy_stmt_type stmt_type);
void invalid_cast(enum toy_val_type val_type, const toy_val *val);
void invalid_val_list_index(toy_val_list *list, toy_num index);
void invalid_string_index(toy_str str, toy_num index);
/* TODO: These should be const */
void too_few_arguments(toy_num expected, toy_expr_list *args);
void too_many_arguments(toy_num expected, toy_expr_list *args);
void invalid_argument_type(enum toy_val_type expected_type, const toy_val *actual_arg);
void invalid_value_type(enum toy_val_type value_type);
void divide_by_zero(void);
void invalid_function_type(enum toy_func_type func_type);

#endif /* TOY_ERRORS_H */
