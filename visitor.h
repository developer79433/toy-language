#ifndef TOY_VISITOR_H
#define TOY_VISITOR_H 1

#include "visitor-types.h"
#include "function-types.h"

void visit_binop(visitor *v, toy_binary_op *binop);
void visit_block(visitor *v, toy_block *block);
void visit_bool(visitor *v, toy_bool b);
void visit_break_stmt(visitor *v);
void visit_continue_stmt(visitor *v);
void visit_expr_list(visitor *v, toy_expr_list *expr_list);
void visit_expr(visitor *v, toy_expr *expr);
void visit_field_ref(visitor *v, toy_field_ref *field_ref);
void visit_for_stmt(visitor *v, toy_for_stmt *for_stmt);
void visit_func_decl(visitor *v, toy_func_decl_stmt *func_decl);
void visit_func_expr(visitor *v, toy_function *func);
void visit_identifier(visitor *v, toy_identifier *identifier);
void visit_if_arm_list(visitor *v, toy_if_arm_list *if_arm_list);
void visit_if_arm(visitor *v, toy_if_arm *if_arm);
void visit_if_stmt(visitor *v, toy_if_stmt *if_stmt);
void visit_map_entry_list(visitor *v, toy_map_entry_list *map_entry_list);
void visit_map_val(visitor *v, map_val *map);
void visit_null_stmt(visitor *v);
void visit_null_val(visitor *v);
void visit_num(visitor *v, toy_num num);
void visit_parameter_list(visitor *v, toy_str_list *parameter_list);
void visit_parameter(visitor *v, toy_str parameter);
void visit_postfix_decrement(visitor *v, toy_postfix_decrement *postfix_decrement);
void visit_postfix_increment(visitor *v, toy_postfix_increment *postfix_increment);
void visit_prefix_decrement(visitor *v, toy_prefix_decrement *prefix_decrement);
void visit_prefix_increment(visitor *v, toy_prefix_increment *prefix_increment);
void visit_return_stmt(visitor *v, toy_return_stmt *return_stmt);
void visit_stmt_list(visitor *v, toy_stmt_list *stmt_list);
void visit_stmt(visitor *v, toy_stmt *stmt);
void visit_string(visitor *v, toy_str str);
void visit_ternary(visitor *v, toy_ternary *ternary);
void visit_unary_op(visitor *v, toy_unary_op *unary_op);
void visit_val_list(visitor *v, toy_val_list *val_list);
void visit_val(visitor *v, toy_val *val);
void visit_var_decl_list(visitor *v, toy_var_decl_list *var_decl_list);
void visit_var_decl(visitor *v, toy_var_decl *var_decl);
void visit_while_stmt(visitor *v, toy_while_stmt *while_stmt);

void default_binop(visitor *v, toy_binary_op *binop);
void default_block(visitor *v, toy_block *block);
void default_bool(visitor *v, toy_bool b);
void default_break_stmt(visitor *v);
void default_continue_stmt(visitor *v);
void default_expr_list(visitor *v, toy_expr_list *expr_list);
void default_expr(visitor *v, toy_expr *expr);
void default_field_ref(visitor *v, toy_field_ref *field_ref);
void default_for_stmt(visitor *v, toy_for_stmt *for_stmt);
void default_func_decl(visitor *v, toy_func_decl_stmt *func_decl);
void default_func_expr(visitor *v, toy_function *func);
void default_identifier(visitor *v, toy_identifier *identifier);
void default_if_arm_list(visitor *v, toy_if_arm_list *if_arm_list);
void default_if_arm(visitor *v, toy_if_arm *if_arm);
void default_if_stmt(visitor *v, toy_if_stmt *if_stmt);
void default_map_entry_list(visitor *v, toy_map_entry_list *map_entry_list);
void default_map_val(visitor *v, map_val *map);
void default_null_stmt(visitor *v);
void default_null_val(visitor *v);
void default_num(visitor *v, toy_num num);
void default_parameter_list(visitor *v, toy_str_list *parameter_list);
void default_parameter(visitor *v, toy_str parameter);
void default_postfix_decrement(visitor *v, toy_postfix_decrement *postfix_decrement);
void default_postfix_increment(visitor *v, toy_postfix_increment *postfix_increment);
void default_prefix_decrement(visitor *v, toy_prefix_decrement *prefix_decrement);
void default_prefix_increment(visitor *v, toy_prefix_increment *prefix_increment);
void default_return_stmt(visitor *v, toy_return_stmt *return_stmt);
void default_stmt_list(visitor *v, toy_stmt_list *stmt_list);
void default_stmt(visitor *v, toy_stmt *stmt);
void default_string(visitor *v, toy_str str);
void default_ternary(visitor *v, toy_ternary *ternary);
void default_unary_op(visitor *v, toy_unary_op *unary_op);
void default_val_list(visitor *v, toy_val_list *val_list);
void default_val(visitor *v, toy_val *val);
void default_var_decl_list(visitor *v, toy_var_decl_list *var_decl_list);
void default_var_decl(visitor *v, toy_var_decl *var_decl);
void default_while_stmt(visitor *v, toy_while_stmt *while_stmt);

#endif /* TOY_VISITOR_H */
