#ifndef TOY_VISITOR_TYPES_H
#define TOY_VISITOR_TYPES_H 1

#include "expr-types.h"
#include "stmt-types.h"

typedef struct visitor_struct visitor;
struct visitor_struct {
    void *cookie; /* For using code to attach arbitrary additional data */
    void (*binop)(visitor *v, toy_binary_op *binop);
    void (*block)(visitor *v, toy_block *block);
    void (*bool)(visitor *v, toy_bool b);
    void (*break_stmt)(struct visitor_struct *v);
    void (*continue_stmt)(struct visitor_struct *v);
    void (*expr)(visitor *v, toy_expr *expr);
    void (*expr_list)(visitor *v, toy_expr_list *expr_list);
    void (*field_ref)(visitor *v, toy_field_ref *field_ref);
    void (*for_stmt)(visitor *v, toy_for_stmt *for_stmt);
    void (*func_decl)(visitor *v, toy_func_decl_stmt *func_decl);
    void (*func_expr)(visitor *v, toy_function *func);
    void (*identifier)(visitor *v, toy_identifier *identifier);
    void (*if_stmt)(visitor *v, toy_if_stmt *if_stmt);
    void (*if_arm)(visitor *v, toy_if_arm *if_arm);
    void (*if_arm_list)(visitor *v, toy_if_arm_list *if_arm_list);
    void (*map_entry_list)(visitor *v, toy_map_entry_list *map_entry_list);
    void (*map_val)(visitor *v, map_val *map);
    void (*null_stmt)(visitor *v);
    void (*null_val)(visitor *v);
    void (*num)(visitor *v, toy_num num);
    void (*parameter)(visitor *v, toy_str parameter);
    void (*parameter_list)(visitor *v, toy_str_list *name_list);
    void (*postfix_decrement)(visitor *v, toy_postfix_decrement *postfix_decrement);
    void (*postfix_increment)(visitor *v, toy_postfix_increment *postfix_increment);
    void (*prefix_decrement)(visitor *v, toy_prefix_decrement *prefix_decrement);
    void (*prefix_increment)(visitor *v, toy_prefix_increment *prefix_increment);
    void (*return_stmt)(visitor *v, toy_return_stmt *return_stmt);
    void (*string)(visitor *v, toy_str str);
    void (*unary_op)(visitor *v, toy_unary_op *unary_op);
    void (*val)(visitor *v, toy_val *val);
    void (*val_list)(visitor *v, toy_val_list *val_list);
    void (*var_decl)(visitor *v, toy_var_decl *var_decl);
    void (*var_decl_list)(visitor *v, toy_var_decl_list *var_decl_list);
    void (*while_stmt)(visitor *v, toy_while_stmt *while_stmt);
    void (*stmt)(visitor *v, toy_stmt *stmt);
    void (*stmt_list)(visitor *v, toy_stmt_list *stmt_list);
    void (*ternary)(visitor *v, toy_ternary *ternary);
};

#endif /* TOY_VISITOR_TYPES_H */
