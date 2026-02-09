#ifndef TOY_AST_VISITOR_H
#define TOY_AST_VISITOR_H 1

#include "ast-visitor-types.h"
#include "function-types.h"

item_callback_result visit_assignment(ast_visitor *v, toy_assignment *assignment);
item_callback_result visit_binop(ast_visitor *v, toy_binary_op *binop);
item_callback_result visit_block(ast_visitor *v, toy_block *block);
item_callback_result visit_bool(ast_visitor *v, toy_bool b);
item_callback_result visit_break_stmt(ast_visitor *v);
item_callback_result visit_collection_lookup(ast_visitor *v, toy_collection_lookup *lookup);
item_callback_result visit_continue_stmt(ast_visitor *v);
item_callback_result visit_expr_list(ast_visitor *v, toy_expr_list *expr_list);
item_callback_result visit_expr(ast_visitor *v, toy_expr *expr);
item_callback_result visit_field_ref(ast_visitor *v, toy_field_ref *field_ref);
item_callback_result visit_for_stmt(ast_visitor *v, toy_for_stmt *for_stmt);
item_callback_result visit_func_closure(ast_visitor *v, func_closure *closure);
item_callback_result visit_func_decl(ast_visitor *v, toy_func_decl_stmt *func_decl);
item_callback_result visit_func_expr(ast_visitor *v, toy_function *func);
item_callback_result visit_identifier(ast_visitor *v, toy_identifier *identifier);
item_callback_result visit_if_arm_list(ast_visitor *v, toy_if_arm_list *if_arm_list);
item_callback_result visit_if_arm(ast_visitor *v, toy_if_arm *if_arm);
item_callback_result visit_if_stmt(ast_visitor *v, toy_if_stmt *if_stmt);
item_callback_result visit_map_entry_list(ast_visitor *v, toy_map_expr_entry_list *map_entry_list);
item_callback_result visit_map_val(ast_visitor *v, map_val *map);
item_callback_result visit_method_call(ast_visitor *v, toy_method_call *method_call);
item_callback_result visit_null_stmt(ast_visitor *v);
item_callback_result visit_null_val(ast_visitor *v);
item_callback_result visit_num(ast_visitor *v, toy_num num);
item_callback_result visit_parameter_list(ast_visitor *v, toy_str_list *parameter_list);
item_callback_result visit_parameter(ast_visitor *v, toy_str parameter);
item_callback_result visit_postfix_decrement(ast_visitor *v, toy_postfix_decrement *postfix_decrement);
item_callback_result visit_postfix_increment(ast_visitor *v, toy_postfix_increment *postfix_increment);
item_callback_result visit_prefix_decrement(ast_visitor *v, toy_prefix_decrement *prefix_decrement);
item_callback_result visit_prefix_increment(ast_visitor *v, toy_prefix_increment *prefix_increment);
item_callback_result visit_return_stmt(ast_visitor *v, toy_return_stmt *return_stmt);
item_callback_result visit_stmt_list(ast_visitor *v, toy_stmt_list *stmt_list);
item_callback_result visit_stmt(ast_visitor *v, toy_stmt *stmt);
item_callback_result visit_string(ast_visitor *v, toy_str str);
item_callback_result visit_ternary(ast_visitor *v, toy_ternary *ternary);
item_callback_result visit_unary_op(ast_visitor *v, toy_unary_op *unary_op);
item_callback_result visit_val_list(ast_visitor *v, toy_val_list *val_list);
item_callback_result visit_val(ast_visitor *v, toy_val *val);
item_callback_result visit_var_decl_list(ast_visitor *v, toy_var_decl_list *var_decl_list);
item_callback_result visit_var_decl(ast_visitor *v, toy_var_decl *var_decl);
item_callback_result visit_while_stmt(ast_visitor *v, toy_while_stmt *while_stmt);

item_callback_result default_assignment(ast_visitor *v, toy_assignment *assignment);
item_callback_result default_binop(ast_visitor *v, toy_binary_op *binop);
item_callback_result default_block(ast_visitor *v, toy_block *block);
item_callback_result default_bool(ast_visitor *v, toy_bool b);
item_callback_result default_break_stmt(ast_visitor *v);
item_callback_result default_collection_lookup(ast_visitor *v, toy_collection_lookup *lookup);
item_callback_result default_continue_stmt(ast_visitor *v);
item_callback_result default_expr_list(ast_visitor *v, toy_expr_list *expr_list);
item_callback_result default_expr(ast_visitor *v, toy_expr *expr);
item_callback_result default_field_ref(ast_visitor *v, toy_field_ref *field_ref);
item_callback_result default_for_stmt(ast_visitor *v, toy_for_stmt *for_stmt);
item_callback_result default_func_closure(ast_visitor *v, func_closure *closure);
item_callback_result default_func_decl(ast_visitor *v, toy_func_decl_stmt *func_decl);
item_callback_result default_func_expr(ast_visitor *v, toy_function *func);
item_callback_result default_identifier(ast_visitor *v, toy_identifier *identifier);
item_callback_result default_if_arm_list(ast_visitor *v, toy_if_arm_list *if_arm_list);
item_callback_result default_if_arm(ast_visitor *v, toy_if_arm *if_arm);
item_callback_result default_if_stmt(ast_visitor *v, toy_if_stmt *if_stmt);
item_callback_result default_map_entry_list(ast_visitor *v, toy_map_expr_entry_list *map_entry_list);
item_callback_result default_map_val(ast_visitor *v, map_val *map);
item_callback_result default_method_call(ast_visitor *v, toy_method_call *method_call);
item_callback_result default_null_stmt(ast_visitor *v);
item_callback_result default_null_val(ast_visitor *v);
item_callback_result default_num(ast_visitor *v, toy_num num);
item_callback_result default_parameter_list(ast_visitor *v, toy_str_list *parameter_list);
item_callback_result default_parameter(ast_visitor *v, toy_str parameter);
item_callback_result default_postfix_decrement(ast_visitor *v, toy_postfix_decrement *postfix_decrement);
item_callback_result default_postfix_increment(ast_visitor *v, toy_postfix_increment *postfix_increment);
item_callback_result default_prefix_decrement(ast_visitor *v, toy_prefix_decrement *prefix_decrement);
item_callback_result default_prefix_increment(ast_visitor *v, toy_prefix_increment *prefix_increment);
item_callback_result default_return_stmt(ast_visitor *v, toy_return_stmt *return_stmt);
item_callback_result default_stmt_list(ast_visitor *v, toy_stmt_list *stmt_list);
item_callback_result default_stmt(ast_visitor *v, toy_stmt *stmt);
item_callback_result default_string(ast_visitor *v, toy_str str);
item_callback_result default_ternary(ast_visitor *v, toy_ternary *ternary);
item_callback_result default_unary_op(ast_visitor *v, toy_unary_op *unary_op);
item_callback_result default_val_list(ast_visitor *v, toy_val_list *val_list);
item_callback_result default_val(ast_visitor *v, toy_val *val);
item_callback_result default_var_decl_list(ast_visitor *v, toy_var_decl_list *var_decl_list);
item_callback_result default_var_decl(ast_visitor *v, toy_var_decl *var_decl);
item_callback_result default_while_stmt(ast_visitor *v, toy_while_stmt *while_stmt);

#endif /* TOY_AST_VISITOR_H */
