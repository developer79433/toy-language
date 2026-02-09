#ifndef TOY_AST_VISITOR_TYPES_H
#define TOY_AST_VISITOR_TYPES_H 1

#include "expr-types.h"
#include "stmt-types.h"

struct ast_visitor_struct;
typedef struct ast_visitor_struct ast_visitor;

typedef item_callback_result (*visit_assignment_func)(ast_visitor *v, toy_assignment *assignment);
typedef item_callback_result (*visit_binop_func)(ast_visitor *v, toy_binary_op *binop);
typedef item_callback_result (*visit_block_func)(ast_visitor *v, toy_block *block);
typedef item_callback_result (*visit_bool_func)(ast_visitor *v, toy_bool b);
typedef item_callback_result (*visit_break_stmt_func)(struct ast_visitor_struct *v);
typedef item_callback_result (*visit_collection_lookup_func)(ast_visitor *v, toy_collection_lookup *lookup);
typedef item_callback_result (*visit_continue_stmt_func)(struct ast_visitor_struct *v);
typedef item_callback_result (*visit_expr_func)(ast_visitor *v, toy_expr *expr);
typedef item_callback_result (*visit_expr_list_func)(ast_visitor *v, toy_expr_list *expr_list);
typedef item_callback_result (*visit_field_ref_func)(ast_visitor *v, toy_field_ref *field_ref);
typedef item_callback_result (*visit_for_stmt_func)(ast_visitor *v, toy_for_stmt *for_stmt);
typedef item_callback_result (*visit_func_call_func)(ast_visitor *v, toy_func_call *func_call);
typedef item_callback_result (*visit_func_closure_func)(ast_visitor *v, func_closure *closure);
typedef item_callback_result (*visit_func_decl_func)(ast_visitor *v, toy_func_decl_stmt *func_decl);
typedef item_callback_result (*visit_func_expr_func)(ast_visitor *v, toy_function *func);
typedef item_callback_result (*visit_identifier_func)(ast_visitor *v, toy_identifier *identifier);
typedef item_callback_result (*visit_if_stmt_func)(ast_visitor *v, toy_if_stmt *if_stmt);
typedef item_callback_result (*visit_if_arm_func)(ast_visitor *v, toy_if_arm *if_arm);
typedef item_callback_result (*visit_if_arm_list_func)(ast_visitor *v, toy_if_arm_list *if_arm_list);
typedef item_callback_result (*visit_map_entry_list_func)(ast_visitor *v, toy_map_expr_entry_list *map_entry_list);
typedef item_callback_result (*visit_map_val_func)(ast_visitor *v, map_val *map);
typedef item_callback_result (*visit_method_call_func)(ast_visitor *v, toy_method_call *method_call);
typedef item_callback_result (*visit_null_stmt_func)(ast_visitor *v);
typedef item_callback_result (*visit_null_val_func)(ast_visitor *v);
typedef item_callback_result (*visit_num_func)(ast_visitor *v, toy_num num);
typedef item_callback_result (*visit_parameter_func)(ast_visitor *v, toy_str parameter);
typedef item_callback_result (*visit_parameter_list_func)(ast_visitor *v, toy_str_list *name_list);
typedef item_callback_result (*visit_postfix_decrement_func)(ast_visitor *v, toy_postfix_decrement *postfix_decrement);
typedef item_callback_result (*visit_postfix_increment_func)(ast_visitor *v, toy_postfix_increment *postfix_increment);
typedef item_callback_result (*visit_prefix_decrement_func)(ast_visitor *v, toy_prefix_decrement *prefix_decrement);
typedef item_callback_result (*visit_prefix_increment_func)(ast_visitor *v, toy_prefix_increment *prefix_increment);
typedef item_callback_result (*visit_return_stmt_func)(ast_visitor *v, toy_return_stmt *return_stmt);
typedef item_callback_result (*visit_string_func)(ast_visitor *v, toy_str str);
typedef item_callback_result (*visit_unary_op_func)(ast_visitor *v, toy_unary_op *unary_op);
typedef item_callback_result (*visit_val_func)(ast_visitor *v, toy_val *val);
typedef item_callback_result (*visit_val_list_func)(ast_visitor *v, toy_val_list *val_list);
typedef item_callback_result (*visit_var_decl_func)(ast_visitor *v, toy_var_decl *var_decl);
typedef item_callback_result (*visit_var_decl_list_func)(ast_visitor *v, toy_var_decl_list *var_decl_list);
typedef item_callback_result (*visit_while_stmt_func)(ast_visitor *v, toy_while_stmt *while_stmt);
typedef item_callback_result (*visit_stmt_func)(ast_visitor *v, toy_stmt *stmt);
typedef item_callback_result (*visit_stmt_list_func)(ast_visitor *v, toy_stmt_list *stmt_list);
typedef item_callback_result (*visit_ternary_func)(ast_visitor *v, toy_ternary *ternary);

typedef struct ast_visitor_struct ast_visitor;
struct ast_visitor_struct {
    visit_assignment_func assignment;
    visit_binop_func binop;
    visit_block_func block;
    visit_bool_func bool;
    visit_break_stmt_func break_stmt;
    visit_collection_lookup_func collection_lookup;
    visit_continue_stmt_func continue_stmt;
    visit_expr_func expr;
    visit_expr_list_func expr_list;
    visit_field_ref_func field_ref;
    visit_for_stmt_func for_stmt;
    visit_func_call_func func_call;
    visit_func_decl_func func_decl;
    visit_func_expr_func func_expr;
    visit_func_closure_func func_closure;
    visit_identifier_func identifier;
    visit_if_stmt_func if_stmt;
    visit_if_arm_func if_arm;
    visit_if_arm_list_func if_arm_list;
    visit_map_entry_list_func map_entry_list;
    visit_map_val_func map_val;
    visit_method_call_func method_call;
    visit_null_stmt_func null_stmt;
    visit_null_val_func null_val;
    visit_num_func num;
    visit_parameter_func parameter;
    visit_parameter_list_func parameter_list;
    visit_postfix_decrement_func postfix_decrement;
    visit_postfix_increment_func postfix_increment;
    visit_prefix_decrement_func prefix_decrement;
    visit_prefix_increment_func prefix_increment;
    visit_return_stmt_func return_stmt;
    visit_string_func string;
    visit_unary_op_func unary_op;
    visit_val_func val;
    visit_val_list_func val_list;
    visit_var_decl_func var_decl;
    visit_var_decl_list_func var_decl_list;
    visit_while_stmt_func while_stmt;
    visit_stmt_func stmt;
    visit_stmt_list_func stmt_list;
    visit_ternary_func ternary;
};

#endif /* TOY_AST_VISITOR_TYPES_H */
