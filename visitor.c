#include <assert.h>

#include "visitor.h"
#include "stmt-list.h"
#include "var-decl-list.h"
#include "if-arm-list.h"
#include "expr-list.h"
#include "val-list.h"
#include "map-val.h"
#include "str-list.h"

void default_func_decl(visitor *v, toy_func_decl_stmt *func_decl)
{
    toy_function *func = &func_decl->func;
    assert(func->type == FUNC_USER_DECLARED);
    visit_string(v, func->name);
    visit_parameter_list(v, func->param_names);
    visit_func_expr(v, func);
}

void visit_func_decl(visitor *v, toy_func_decl_stmt *func_decl)
{
    if (v->func_decl) {
        v->func_decl(v, func_decl);
    } else {
        default_func_decl(v, func_decl);
    }
}

void default_break_stmt(visitor *v)
{
    /* NOP */
}

void visit_break_stmt(visitor *v)
{
    if (v->break_stmt) {
        v->break_stmt(v);
    } else {
        default_break_stmt(v);
    }
}

void default_continue_stmt(visitor *v)
{
    /* NOP */
}

void visit_continue_stmt(visitor *v)
{
    if (v->continue_stmt) {
        v->continue_stmt(v);
    } else {
        default_continue_stmt(v);
    }
}

void default_binop(visitor *v, toy_binary_op *binop)
{
    visit_expr(v, binop->arg1);
    visit_expr(v, binop->arg2);
}

void visit_binop(visitor *v, toy_binary_op *binop)
{
    if (v->binop) {
        v->binop(v, binop);
    } else {
        default_binop(v, binop);
    }
}

void default_parameter(visitor *v, toy_str parameter)
{
    visit_string(v, parameter);
}

void visit_parameter(visitor *v, toy_str parameter)
{
    if (v->parameter) {
        v->parameter(v, parameter);
    } else {
        default_parameter(v, parameter);
    }
}


typedef struct parameter_cb_args_struct {
    visitor *v;
} parameter_cb_args;

static item_callback_result parameter_cb(void *cookie, size_t index, toy_str_list *item)
{
    parameter_cb_args *args = (parameter_cb_args *) cookie;
    toy_str str = str_list_payload(item);
    visit_parameter(args->v, str);
    return ENUMERATION_COMPLETE;
}

void default_parameter_list(visitor *v, toy_str_list *parameter_list)
{
    parameter_cb_args args = { .v = v };
    enumeration_result res = str_list_foreach(parameter_list, parameter_cb, &args);
    assert(ENUMERATION_COMPLETE == res);
}

void visit_parameter_list(visitor *v, toy_str_list *name_list)
{
    if (v->parameter_list) {
        v->parameter_list(v, name_list);
    } else {
        default_parameter_list(v, name_list);
    }
}

void default_identifier(visitor *v, toy_identifier *identifier)
{
    visit_string(v, identifier->name);
}

void visit_identifier(visitor *v, toy_identifier *identifier)
{
    if (v->identifier) {
        v->identifier(v, identifier);
    } else {
        default_identifier(v, identifier);
    }
}

void default_field_ref(visitor *v, toy_field_ref *field_ref)
{
    visit_identifier(v, &field_ref->id);
    visit_string(v, field_ref->field_name);
}

void visit_field_ref(visitor *v, toy_field_ref *field_ref)
{
    if (v->field_ref) {
        v->field_ref(v, field_ref);
    } else {
        default_field_ref(v, field_ref);
    }
}

typedef struct expr_cb_args_struct {
    visitor *v;
} expr_cb_args;

static item_callback_result expr_cb(void *cookie, size_t index, toy_expr_list *expr_list)
{
    expr_cb_args *args = (expr_cb_args *) cookie;
    toy_expr *expr = expr_list_payload(expr_list);
    visit_expr(args->v, expr);
    return CONTINUE_ENUMERATION;
}

void default_expr_list(visitor *v, toy_expr_list *expr_list)
{
    expr_cb_args args = { .v = v };
    expr_list_foreach(expr_list, expr_cb, &args);
}

void visit_expr_list(visitor *v, toy_expr_list *expr_list)
{
    if (v->expr_list) {
        v->expr_list(v, expr_list);
    } else {
        default_expr_list(v, expr_list);
    }
}

void default_bool(visitor *v, toy_bool b)
{
    /* NOP */
}

void visit_bool(visitor *v, toy_bool b)
{
    if (v->bool) {
        v->bool(v, b);
    } else {
        default_bool(v, b);
    }
}

typedef struct val_cb_args_struct {
    visitor *v;
} val_cb_args;

static item_callback_result val_cb(void *cookie, size_t index, toy_val_list *item)
{
    val_cb_args *args = (val_cb_args *) cookie;
    toy_val *val = val_list_payload(item);
    visit_val(args->v, val);
    return CONTINUE_ENUMERATION;
}

void default_val_list(visitor *v, toy_val_list *val_list)
{
    val_cb_args args = { .v = v };
    enumeration_result res = val_list_foreach(val_list, val_cb, &args);
    assert(ENUMERATION_COMPLETE == res);
}

void visit_val_list(visitor *v, toy_val_list *val_list)
{
    if (v->val_list) {
        v->val_list(v, val_list);
    } else {
        default_val_list(v, val_list);
    }
}

typedef struct map_val_cb_args_struct {
    visitor *v;
} map_val_cb_args;

static item_callback_result map_val_cb(void *cookie, map_val_entry *entry)
{
    map_val_cb_args *args = (map_val_cb_args *) cookie;
    visit_string(args->v, entry->key);
    visit_val(args->v, &entry->value);
    return CONTINUE_ENUMERATION;
}

void default_map_val(visitor *v, map_val *map)
{
    map_val_cb_args args = { .v = v };
    enumeration_result res = map_val_foreach(map, map_val_cb, &args);
    assert(ENUMERATION_COMPLETE == res);
}

void visit_map_val(visitor *v, map_val *map)
{
    if (v->map_val) {
        v->map_val(v, map);
    } else {
        default_map_val(v, map);
    }
}

void default_null_val(visitor *v)
{
    /* NOP */
}

void visit_null_val(visitor *v)
{
    if (v->null_val) {
        v->null_val(v);
    } else {
        default_null_val(v);
    }
}

void default_num(visitor *v, toy_num num)
{
    /* NOP */
}

void visit_num(visitor *v, toy_num num)
{
    if (v->num) {
        v->num(v, num);
    } else {
        default_num(v, num);
    }
}

void default_string(visitor *v, toy_str str)
{
    /* NOP */
}

void visit_string(visitor *v, toy_str str)
{
    if (v->string) {
        v->string(v, str);
    } else {
        default_string(v, str);
    }
}

void default_val(visitor *v, toy_val *val)
{
    switch (val->type) {
    case VAL_BOOL:
        visit_bool(v, val->boolean);
        break;
    case VAL_FUNC:
        visit_func_expr(v, val->func);
        break;
    case VAL_LIST:
        visit_val_list(v, val->list);
        break;
    case VAL_MAP:
        visit_map_val(v, val->map);
        break;
    case VAL_NULL:
        visit_null_val(v);
        break;
    case VAL_NUM:
        visit_num(v, val->num);
        break;
    case VAL_STR:
        visit_string(v, val->str);
        break;
    default:
        assert(0);
        break;
    }
}

void visit_val(visitor *v, toy_val *val)
{
    if (*v->val) {
        v->val(v, val);
    } else {
        default_val(v, val);
    }
}

typedef struct map_entry_cb_args_struct {
    visitor *v;
} map_entry_cb_args;

static item_callback_result map_entry_cb(void *cookie, size_t index, toy_map_entry_list *list)
{
    map_entry_cb_args *args = (map_entry_cb_args *) cookie;
    toy_map_entry *entry = map_entry_list_payload(list);
    visit_string(args->v, entry->key);
    visit_expr(args->v, entry->value);
    return CONTINUE_ENUMERATION;
}

void default_map_entry_list(visitor *v, toy_map_entry_list *map_entry_list)
{
    map_entry_cb_args args = { .v = v };
    enumeration_result res = map_entry_list_foreach(map_entry_list, map_entry_cb, &args);
    assert(ENUMERATION_COMPLETE == res);
}

void visit_map_entry_list(visitor *v, toy_map_entry_list *map_entry_list)
{
    if (v->map_entry_list) {
        v->map_entry_list(v, map_entry_list);
    } else {
        default_map_entry_list(v, map_entry_list);
    }
}

void default_unary_op(visitor *v, toy_unary_op *unary_op)
{
    visit_expr(v, unary_op->arg);
}

void visit_unary_op(visitor *v, toy_unary_op *unary_op)
{
    if (v->unary_op) {
        v->unary_op(v, unary_op);
    } else {
        default_unary_op(v, unary_op);
    }
}

void default_postfix_decrement(visitor *v, toy_postfix_decrement *postfix_decrement)
{
    visit_identifier(v, &postfix_decrement->id);
}

void visit_postfix_decrement(visitor *v, toy_postfix_decrement *postfix_decrement)
{
    if (v->postfix_decrement) {
        v->postfix_decrement(v, postfix_decrement);
    } else {
        default_postfix_decrement(v, postfix_decrement);
    }
}

void default_postfix_increment(visitor *v, toy_postfix_increment *postfix_increment)
{
    visit_identifier(v, &postfix_increment->id);
}

void visit_postfix_increment(visitor *v, toy_postfix_increment *postfix_increment)
{
    if (v->postfix_increment) {
        v->postfix_increment(v, postfix_increment);
    } else {
        default_postfix_increment(v, postfix_increment);
    }
}

void default_prefix_decrement(visitor *v, toy_prefix_decrement *prefix_decrement)
{
    visit_identifier(v, &prefix_decrement->id);
}

void visit_prefix_decrement(visitor *v, toy_prefix_decrement *prefix_decrement)
{
    if (v->prefix_decrement) {
        v->prefix_decrement(v, prefix_decrement);
    } else {
        default_prefix_decrement(v, prefix_decrement);
    }
}

void default_prefix_increment(visitor *v, toy_prefix_increment *prefix_increment)
{
    visit_identifier(v, &prefix_increment->id);
}

void visit_prefix_increment(visitor *v, toy_prefix_increment *prefix_increment)
{
    if (v->prefix_increment) {
        v->prefix_increment(v, prefix_increment);
    } else {
        default_prefix_increment(v, prefix_increment);
    }
}

void default_ternary(visitor *v, toy_ternary *ternary)
{
    visit_expr(v, ternary->condition);
    visit_expr(v, ternary->if_true);
    visit_expr(v, ternary->if_false);
}

void visit_ternary(visitor *v, toy_ternary *ternary)
{
    if (v->ternary) {
        v->ternary(v, ternary);
    } else {
        default_ternary(v, ternary);
    }
}

void default_expr(visitor *v, toy_expr *expr)
{
    switch (expr->type) {
    case EXPR_AND:
        visit_binop(v, &expr->binary_op);
        break;
    case EXPR_ASSIGN:
        toy_assignment *assignment = &expr->assignment;
        visit_identifier(v, &assignment->id);
        visit_expr(v, assignment->rhs);
        break;
    case EXPR_COLLECTION_LOOKUP:
        toy_collection_lookup *lookup = &expr->collection_lookup;
        visit_identifier(v, &lookup->id);
        visit_expr(v, lookup->rhs);
        break;
    case EXPR_COMMA:
    case EXPR_DIV:
    case EXPR_EQUAL:
    case EXPR_EXPONENT:
        visit_binop(v, &expr->binary_op);
        break;
    case EXPR_FIELD_REF:
        toy_field_ref *field_ref = &expr->field_ref;
        visit_field_ref(v, field_ref);
        break;
    case EXPR_FUNC_CALL:
        toy_func_call *call = &expr->func_call;
        visit_identifier(v, &call->id);
        visit_expr_list(v, call->args);
        break;
    case EXPR_GT:
    case EXPR_GTE:
        visit_binop(v, &expr->binary_op);
        break;
    case EXPR_IDENTIFIER:
        toy_identifier *identifier = &expr->id;
        visit_identifier(v, identifier);
        break;
    case EXPR_IN:
        visit_binop(v, &expr->binary_op);
        break;
    case EXPR_LIST:
        visit_expr_list(v, expr->list);
        break;
    case EXPR_LITERAL:
        toy_val *val = &expr->val;
        visit_val(v, val);
        break;
    case EXPR_LT:
    case EXPR_LTE:
        visit_binop(v, &expr->binary_op);
        break;
    case EXPR_MAP:
        toy_map_entry_list *entry_list = expr->map;
        visit_map_entry_list(v, entry_list);
        break;
    case EXPR_METHOD_CALL:
        toy_method_call *method_call = &expr->method_call;
        visit_identifier(v, &method_call->id);
        visit_string(v, method_call->method_name);
        visit_expr_list(v, method_call->args);
        break;
    case EXPR_MINUS:
    case EXPR_MODULUS:
    case EXPR_MUL:
    case EXPR_NEQUAL:
        visit_binop(v, &expr->binary_op);
        break;
    case EXPR_NOT:
        visit_unary_op(v, &expr->unary_op);
        break;
    case EXPR_OR:
    case EXPR_PLUS:
        visit_binop(v, &expr->binary_op);
        break;
    case EXPR_POSTFIX_DECREMENT:
        visit_postfix_decrement(v, &expr->postfix_decrement);
        break;
    case EXPR_POSTFIX_INCREMENT:
        visit_postfix_increment(v, &expr->postfix_increment);
        break;
    case EXPR_PREFIX_DECREMENT:
        visit_prefix_decrement(v, &expr->prefix_decrement);
        break;
    case EXPR_PREFIX_INCREMENT:
        visit_prefix_increment(v, &expr->prefix_increment);
        break;
    case EXPR_TERNARY:
        visit_ternary(v, &expr->ternary);
        break;
    case EXPR_UNEG:
        visit_unary_op(v, &expr->unary_op);
        break;
    default:
        assert(0);
        break;
    }
}

void visit_expr(visitor *v, toy_expr *expr)
{
    if (v->expr) {
        v->expr(v, expr);
    } else {
        default_expr(v, expr);
    }
}

void default_for_stmt(visitor *v, toy_for_stmt *for_stmt)
{
    visit_stmt(v, for_stmt->at_end);
    visit_stmt(v, for_stmt->at_start);
    visit_expr(v, for_stmt->condition);
    visit_block(v, &for_stmt->body);
}

void visit_for_stmt(visitor *v, toy_for_stmt *for_stmt)
{
    if (v->for_stmt) {
        v->for_stmt(v, for_stmt);
    } else {
        default_for_stmt(v, for_stmt);
    }
}

void default_if_arm(visitor *v, toy_if_arm *if_arm)
{
    visit_expr(v, if_arm->condition);
    visit_block(v, &if_arm->code);
}

void visit_if_arm(visitor *v, toy_if_arm *if_arm)
{
    if (v->if_arm) {
        v->if_arm(v, if_arm);
    } else {
        default_if_arm(v, if_arm);
    }
}

typedef struct if_arm_cb_args_struct {
    visitor *v;
} if_arm_cb_args;

static item_callback_result if_arm_cb(void *cookie, size_t index, toy_if_arm_list *item)
{
    if_arm_cb_args *args = (if_arm_cb_args *) cookie;
    toy_if_arm *if_arm = if_arm_list_payload(item);
    visit_if_arm(args->v, if_arm);
    return CONTINUE_ENUMERATION;
}

void default_if_arm_list(visitor *v, toy_if_arm_list *if_arm_list)
{
    if_arm_cb_args args = { .v = v };
    enumeration_result res = if_arm_list_foreach(if_arm_list, if_arm_cb, &args);
    assert(ENUMERATION_COMPLETE == res);
}

void visit_if_arm_list(visitor *v, toy_if_arm_list *if_arm_list)
{
    if (v->if_arm_list) {
        v->if_arm_list(v, if_arm_list);
    } else {
        default_if_arm_list(v, if_arm_list);
    }
}

void default_if_stmt(visitor *v, toy_if_stmt *if_stmt)
{
    visit_if_arm_list(v, if_stmt->arms);
    visit_block(v, &if_stmt->elsepart);
}

void visit_if_stmt(visitor *v, toy_if_stmt *if_stmt)
{
    if (v->if_stmt) {
        v->if_stmt(v, if_stmt);
    } else {
        default_if_stmt(v, if_stmt);
    }
}

void default_null_stmt(visitor *v)
{
    /* NOP */
}

void visit_null_stmt(visitor *v)
{
    if (v->null_stmt) {
        v->null_stmt(v);
    } else {
        default_null_stmt(v);
    }
}

void default_return_stmt(visitor *v, toy_return_stmt *return_stmt)
{
    visit_expr(v, return_stmt->expr);
}

void visit_return_stmt(visitor *v, toy_return_stmt *return_stmt)
{
    if (v->return_stmt) {
        v->return_stmt(v, return_stmt);
    } else {
        default_return_stmt(v, return_stmt);
    }
}

void default_var_decl(visitor *v, toy_var_decl *var_decl)
{
    visit_string(v, var_decl->name);
    visit_expr(v, var_decl->value);
}

void visit_var_decl(visitor *v, toy_var_decl *var_decl)
{
    if (v->var_decl) {
        v->var_decl(v, var_decl);
    } else {
        default_var_decl(v, var_decl);
    }
}

typedef struct var_decl_args_struct {
    visitor *v;
} var_decl_args;

static item_callback_result var_decl_callback(void *cookie, size_t index, toy_var_decl_list *item)
{
    var_decl_args *args = (var_decl_args *) cookie;
    toy_var_decl *var_decl = var_decl_list_payload(item);
    visit_var_decl(args->v, var_decl);
    return CONTINUE_ENUMERATION;
}

void default_var_decl_list(visitor *v, toy_var_decl_list *var_decl_list)
{
    var_decl_args args = { .v = v };
    enumeration_result res = var_decl_list_foreach(var_decl_list, var_decl_callback, &args);
    assert(res == ENUMERATION_COMPLETE);
}

void visit_var_decl_list(visitor *v, toy_var_decl_list *var_decl_list)
{
    if (v->var_decl_list) {
        v->var_decl_list(v, var_decl_list);
    } else {
        default_var_decl_list(v, var_decl_list);
    }
}

void default_while_stmt(visitor *v, toy_while_stmt *while_stmt)
{
    visit_expr(v, while_stmt->condition);
    visit_block(v, &while_stmt->body);
}

void visit_while_stmt(visitor *v, toy_while_stmt *while_stmt)
{
    if (v->while_stmt) {
        v->while_stmt(v, while_stmt);
    } else {
        default_while_stmt(v, while_stmt);
    }
}

void default_stmt(visitor *v, toy_stmt *stmt)
{
    switch (stmt->type) {
    case STMT_BLOCK:
        toy_block *block = &stmt->block_stmt.block;
        visit_block(v, block);
        break;
    case STMT_BREAK:
        visit_break_stmt(v);
        break;
    case STMT_CONTINUE:
        visit_continue_stmt(v);
        break;
    case STMT_EXPR:
        toy_expr *expr = stmt->expr_stmt.expr;
        visit_expr(v, expr);
        break;
    case STMT_FOR:
        toy_for_stmt *for_stmt = &stmt->for_stmt;
        visit_for_stmt(v, for_stmt);
        break;
    case STMT_FUNC_DECL:
        toy_func_decl_stmt *fdecl = &stmt->func_decl_stmt;
        visit_func_decl(v, fdecl);
        break;
    case STMT_IF:
        toy_if_stmt *if_stmt = &stmt->if_stmt;
        visit_if_stmt(v, if_stmt);
        break;
    case STMT_NULL:
        visit_null_stmt(v);
        break;
    case STMT_RETURN:
        toy_return_stmt *return_stmt = &stmt->return_stmt;
        visit_return_stmt(v, return_stmt);
        break;
    case STMT_VAR_DECL:
        toy_var_decl_list *var_decl_list = &stmt->var_decl_stmt;
        visit_var_decl_list(v, var_decl_list);
        break;
    case STMT_WHILE:
        toy_while_stmt *while_stmt = &stmt->while_stmt;
        visit_while_stmt(v, while_stmt);
        break;
    default:
        assert(0);
        break;
    }
}

void visit_stmt(visitor *v, toy_stmt *stmt)
{
    if (v->stmt) {
        v->stmt(v, stmt);
    } else {
        default_stmt(v, stmt);
    }
}

typedef struct visit_stmt_args_struct {
    visitor *v;
} visit_stmt_args;

static item_callback_result visit_stmt_callback(void *cookie, size_t index, toy_stmt_list *item)
{
    visit_stmt_args *args = (visit_stmt_args *) cookie;
    toy_stmt *stmt = stmt_list_payload(item);
    visit_stmt(args->v, stmt);
    return CONTINUE_ENUMERATION;
}

void default_stmt_list(visitor *v, toy_stmt_list *stmt_list)
{
    visit_stmt_args args = { .v = v };
    enumeration_result res = stmt_list_foreach(stmt_list, visit_stmt_callback, &args);
    assert(ENUMERATION_COMPLETE == res);
}

void visit_stmt_list(visitor *v, toy_stmt_list *stmt_list)
{
    if (v->stmt_list) {
        v->stmt_list(v, stmt_list);
    } else {
        default_stmt_list(v, stmt_list);
    }
}

void default_block(visitor *v, toy_block *block)
{
    visit_stmt_list(v, block->stmts);
}

void visit_block(visitor *v, toy_block *block)
{
    if (v->block) {
        v->block(v, block);
    } else {
        default_block(v, block);
    }
}

void default_func_expr(visitor *v, toy_function *func)
{
    visit_block(v, &func->code);
}

void visit_func_expr(visitor *v, toy_function *func)
{
    if (v->func_expr) {
        v->func_expr(v, func);
    } else {
        default_func_expr(v, func);
    }
}
