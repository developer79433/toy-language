#ifndef TOY_AST_VISITOR_TYPES_H
#define TOY_AST_VISITOR_TYPES_H 1

#include "expr-types.h"
#include "stmt-types.h"

typedef struct ast_visitor_struct ast_visitor;
struct ast_visitor_struct {
    item_callback_result (*assignment)(ast_visitor *v, toy_assignment *assignment);
    item_callback_result (*binop)(ast_visitor *v, toy_binary_op *binop);
    item_callback_result (*block)(ast_visitor *v, toy_block *block);
    item_callback_result (*bool)(ast_visitor *v, toy_bool b);
    item_callback_result (*break_stmt)(struct ast_visitor_struct *v);
    item_callback_result (*collection_lookup)(ast_visitor *v, toy_collection_lookup *lookup);
    item_callback_result (*continue_stmt)(struct ast_visitor_struct *v);
    item_callback_result (*expr)(ast_visitor *v, toy_expr *expr);
    item_callback_result (*expr_list)(ast_visitor *v, toy_expr_list *expr_list);
    item_callback_result (*field_ref)(ast_visitor *v, toy_field_ref *field_ref);
    item_callback_result (*for_stmt)(ast_visitor *v, toy_for_stmt *for_stmt);
    item_callback_result (*func_call)(ast_visitor *v, toy_func_call *func_call);
    item_callback_result (*func_decl)(ast_visitor *v, toy_func_decl_stmt *func_decl);
    item_callback_result (*func_expr)(ast_visitor *v, toy_function *func);
    item_callback_result (*identifier)(ast_visitor *v, toy_identifier *identifier);
    item_callback_result (*if_stmt)(ast_visitor *v, toy_if_stmt *if_stmt);
    item_callback_result (*if_arm)(ast_visitor *v, toy_if_arm *if_arm);
    item_callback_result (*if_arm_list)(ast_visitor *v, toy_if_arm_list *if_arm_list);
    item_callback_result (*map_entry_list)(ast_visitor *v, toy_map_entry_list *map_entry_list);
    item_callback_result (*map_val)(ast_visitor *v, map_val *map);
    item_callback_result (*method_call)(ast_visitor *v, toy_method_call *method_call);
    item_callback_result (*null_stmt)(ast_visitor *v);
    item_callback_result (*null_val)(ast_visitor *v);
    item_callback_result (*num)(ast_visitor *v, toy_num num);
    item_callback_result (*parameter)(ast_visitor *v, toy_str parameter);
    item_callback_result (*parameter_list)(ast_visitor *v, toy_str_list *name_list);
    item_callback_result (*postfix_decrement)(ast_visitor *v, toy_postfix_decrement *postfix_decrement);
    item_callback_result (*postfix_increment)(ast_visitor *v, toy_postfix_increment *postfix_increment);
    item_callback_result (*prefix_decrement)(ast_visitor *v, toy_prefix_decrement *prefix_decrement);
    item_callback_result (*prefix_increment)(ast_visitor *v, toy_prefix_increment *prefix_increment);
    item_callback_result (*return_stmt)(ast_visitor *v, toy_return_stmt *return_stmt);
    item_callback_result (*string)(ast_visitor *v, toy_str str);
    item_callback_result (*unary_op)(ast_visitor *v, toy_unary_op *unary_op);
    item_callback_result (*val)(ast_visitor *v, toy_val *val);
    item_callback_result (*val_list)(ast_visitor *v, toy_val_list *val_list);
    item_callback_result (*var_decl)(ast_visitor *v, toy_var_decl *var_decl);
    item_callback_result (*var_decl_list)(ast_visitor *v, toy_var_decl_list *var_decl_list);
    item_callback_result (*while_stmt)(ast_visitor *v, toy_while_stmt *while_stmt);
    item_callback_result (*stmt)(ast_visitor *v, toy_stmt *stmt);
    item_callback_result (*stmt_list)(ast_visitor *v, toy_stmt_list *stmt_list);
    item_callback_result (*ternary)(ast_visitor *v, toy_ternary *ternary);
};

#endif /* TOY_AST_VISITOR_TYPES_H */
