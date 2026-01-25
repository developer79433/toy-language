#include <assert.h>

#include "ast-visitor.h"
#include "stmt-list.h"
#include "var-decl-list.h"
#include "if-arm-list.h"
#include "expr-list.h"
#include "val-list.h"
#include "map-val.h"
#include "str-list.h"
#include "list-visitor.h"
#include "map-visitor.h"

typedef struct list_ast_visitor_struct {
    list_visitor list_vis;
    ast_visitor *ast_vis;
} list_ast_visitor;

typedef struct map_ast_visitor_struct {
    map_visitor map_vis;
    ast_visitor *ast_vis;
} map_ast_visitor;

item_callback_result default_func_decl(ast_visitor *v, toy_func_decl_stmt *func_decl)
{
    toy_function *func = func_decl->func;
    assert(func->type == FUNC_USER_DECLARED);
    item_callback_result res;
    res = visit_string(v, func->name);
    if (STOP_ENUMERATION == res) {
        return res;
    }
    res = visit_parameter_list(v, func->param_names);
    if (STOP_ENUMERATION == res) {
        return res;
    }
    res = visit_func_expr(v, func);
    if (STOP_ENUMERATION == res) {
        return res;
    }
    return CONTINUE_ENUMERATION;
}

item_callback_result visit_func_decl(ast_visitor *v, toy_func_decl_stmt *func_decl)
{
    if (v->func_decl) {
        return v->func_decl(v, func_decl);
    }
    return default_func_decl(v, func_decl);
}

item_callback_result default_break_stmt(ast_visitor *v)
{
    /* NOP */
    return CONTINUE_ENUMERATION;
}

item_callback_result visit_break_stmt(ast_visitor *v)
{
    if (v->break_stmt) {
        return v->break_stmt(v);
    }
    return default_break_stmt(v);
}

item_callback_result default_continue_stmt(ast_visitor *v)
{
    /* NOP */
    return CONTINUE_ENUMERATION;
}

item_callback_result visit_continue_stmt(ast_visitor *v)
{
    if (v->continue_stmt) {
        return v->continue_stmt(v);
    }
    return default_continue_stmt(v);
}

item_callback_result default_binop(ast_visitor *v, toy_binary_op *binop)
{
    item_callback_result res;
    res = visit_expr(v, binop->arg1);
    if (STOP_ENUMERATION == res) {
        return res;
    }
    res = visit_expr(v, binop->arg2);
    if (STOP_ENUMERATION == res) {
        return res;
    }
    return CONTINUE_ENUMERATION;
}

item_callback_result visit_binop(ast_visitor *v, toy_binary_op *binop)
{
    if (v->binop) {
        return v->binop(v, binop);
    }
    return default_binop(v, binop);
}

item_callback_result default_parameter(ast_visitor *v, toy_str parameter)
{
    return visit_string(v, parameter);
}

item_callback_result visit_parameter(ast_visitor *v, toy_str parameter)
{
    if (v->parameter) {
        return v->parameter(v, parameter);
    }
    return default_parameter(v, parameter);
}

static item_callback_result parameter_cb(list_ast_visitor *v, size_t index, toy_str_list *item)
{
    toy_str str = str_list_payload(item);
    return visit_parameter(v->ast_vis, str);
}

item_callback_result default_parameter_list(ast_visitor *v, toy_str_list *parameter_list)
{
    list_ast_visitor param_visitor = { .list_vis.visit_entry = (list_entry_visit_func) parameter_cb, .ast_vis = v };
    enumeration_result res = list_visitor_visit_list((list_visitor *) &param_visitor, (generic_list *) parameter_list);
    if (ENUMERATION_COMPLETE == res) {
        return CONTINUE_ENUMERATION;
    }
    assert(ENUMERATION_INTERRUPTED == res);
    return STOP_ENUMERATION;
}

item_callback_result visit_parameter_list(ast_visitor *v, toy_str_list *name_list)
{
    if (v->parameter_list) {
        return v->parameter_list(v, name_list);
    }
    return default_parameter_list(v, name_list);
}

item_callback_result default_identifier(ast_visitor *v, toy_identifier *identifier)
{
    return visit_string(v, identifier->name);
}

item_callback_result visit_identifier(ast_visitor *v, toy_identifier *identifier)
{
    if (v->identifier) {
        return v->identifier(v, identifier);
    }
    return default_identifier(v, identifier);
}

item_callback_result default_field_ref(ast_visitor *v, toy_field_ref *field_ref)
{
    item_callback_result res;
    res = visit_identifier(v, &field_ref->id);
    if (STOP_ENUMERATION == res) {
        return res;
    }
    return visit_string(v, field_ref->field_name);
}

item_callback_result visit_field_ref(ast_visitor *v, toy_field_ref *field_ref)
{
    if (v->field_ref) {
        return v->field_ref(v, field_ref);
    }
    return default_field_ref(v, field_ref);
}

static item_callback_result expr_cb(list_ast_visitor *v, size_t index, toy_expr_list *expr_list)
{
    toy_expr *expr = expr_list_payload(expr_list);
    return visit_expr(v->ast_vis, expr);
}

item_callback_result default_expr_list(ast_visitor *v, toy_expr_list *expr_list)
{
    list_ast_visitor expr_visitor = { .list_vis.visit_entry = (list_entry_visit_func) expr_cb, .ast_vis = v };
    enumeration_result res = list_visitor_visit_list((list_visitor *) &expr_visitor, (generic_list *) expr_list);
    if (ENUMERATION_COMPLETE == res) {
        return CONTINUE_ENUMERATION;
    }
    assert(ENUMERATION_INTERRUPTED == res);
    return STOP_ENUMERATION;
}

item_callback_result visit_expr_list(ast_visitor *v, toy_expr_list *expr_list)
{
    if (v->expr_list) {
        return v->expr_list(v, expr_list);
    }
    return default_expr_list(v, expr_list);
}

item_callback_result default_bool(ast_visitor *v, toy_bool b)
{
    /* NOP */
    return CONTINUE_ENUMERATION;
}

item_callback_result visit_bool(ast_visitor *v, toy_bool b)
{
    if (v->bool) {
        return v->bool(v, b);
    }
    return default_bool(v, b);
}

item_callback_result default_func_call(ast_visitor *v, toy_func_call *func_call)
{
    item_callback_result res;
    res = visit_identifier(v, &func_call->id);
    if (STOP_ENUMERATION == res) {
        return res;
    }
    return visit_expr_list(v, func_call->args);
}

item_callback_result visit_func_call(ast_visitor *v, toy_func_call *func_call)
{
    if (v->func_call) {
        return v->func_call(v, func_call);
    }
    return default_func_call(v, func_call);
}

static item_callback_result val_cb(list_ast_visitor *v, size_t index, toy_val_list *item)
{
    toy_val *val = val_list_payload(item);
    return visit_val(v->ast_vis, val);
}

item_callback_result default_val_list(ast_visitor *v, toy_val_list *val_list)
{
    list_ast_visitor val_visitor = { .list_vis.visit_entry = (list_entry_visit_func) val_cb, .ast_vis = v };
    enumeration_result res = list_visitor_visit_list((list_visitor *) &val_visitor, (generic_list *) val_list);
    if (ENUMERATION_COMPLETE == res) {
        return CONTINUE_ENUMERATION;
    }
    assert(ENUMERATION_INTERRUPTED == res);
    return STOP_ENUMERATION;
}

item_callback_result visit_val_list(ast_visitor *v, toy_val_list *val_list)
{
    if (v->val_list) {
        return v->val_list(v, val_list);
    }
    return default_val_list(v, val_list);
}

static item_callback_result map_val_cb(map_ast_visitor *v, map_val_entry *entry)
{
    ast_visitor *ast_vis = v->ast_vis;
    item_callback_result res;
    res = visit_string(ast_vis, entry->key);
    if (STOP_ENUMERATION == res) {
        return res;
    }
    return visit_val(ast_vis, &entry->value);
}

item_callback_result default_map_val(ast_visitor *v, map_val *map)
{
    map_ast_visitor val_visitor = { .map_vis.visit_entry = (map_entry_visit_func) map_val_cb, .ast_vis = v };
    enumeration_result res = map_visitor_visit_map((map_visitor *) &val_visitor, (generic_map *) map);
    if (ENUMERATION_COMPLETE == res) {
        return CONTINUE_ENUMERATION;
    }
    assert(ENUMERATION_INTERRUPTED == res);
    return STOP_ENUMERATION;
}

item_callback_result visit_map_val(ast_visitor *v, map_val *map)
{
    if (v->map_val) {
        return v->map_val(v, map);
    }
    return default_map_val(v, map);
}

item_callback_result default_null_val(ast_visitor *v)
{
    /* NOP */
    return CONTINUE_ENUMERATION;
}

item_callback_result visit_null_val(ast_visitor *v)
{
    if (v->null_val) {
        return v->null_val(v);
    }
    return default_null_val(v);
}

item_callback_result default_num(ast_visitor *v, toy_num num)
{
    /* NOP */
    return CONTINUE_ENUMERATION;
}

item_callback_result visit_num(ast_visitor *v, toy_num num)
{
    if (v->num) {
        return v->num(v, num);
    }
    return default_num(v, num);
}

item_callback_result default_string(ast_visitor *v, toy_str str)
{
    /* NOP */
    return CONTINUE_ENUMERATION;
}

item_callback_result visit_string(ast_visitor *v, toy_str str)
{
    if (v->string) {
        return v->string(v, str);
    }
    return default_string(v, str);
}

item_callback_result default_val(ast_visitor *v, toy_val *val)
{
    switch (val->type) {
    case VAL_BOOL:
        return visit_bool(v, val->boolean);
    case VAL_FUNC:
        return visit_func_expr(v, val->func);
    case VAL_LIST:
        return visit_val_list(v, val->list);
    case VAL_MAP:
        return visit_map_val(v, val->map);
    case VAL_NULL:
        return visit_null_val(v);
    case VAL_NUM:
        return visit_num(v, val->num);
    case VAL_STR:
        return visit_string(v, val->str);
    default:
        assert(0);
        break;
    }
}

item_callback_result visit_val(ast_visitor *v, toy_val *val)
{
    if (*v->val) {
        return v->val(v, val);
    }
    return default_val(v, val);
}

static item_callback_result map_entry_cb(map_ast_visitor *v, size_t index, toy_map_entry_list *list)
{
    ast_visitor *ast_vis = v->ast_vis;
    toy_map_entry *entry = map_entry_list_payload(list);
    item_callback_result res;
    res = visit_string(ast_vis, entry->key);
    if (STOP_ENUMERATION == res) {
        return res;
    }
    return visit_expr(ast_vis, entry->expr);
}

item_callback_result default_map_entry_list(ast_visitor *v, toy_map_entry_list *map_entry_list)
{
    map_ast_visitor entry_visitor = { .map_vis.visit_entry = (map_entry_visit_func) map_entry_cb, .ast_vis = v };
    enumeration_result res = map_visitor_visit_map((map_visitor *) &entry_visitor, (generic_map *) map_entry_list);
    if (ENUMERATION_COMPLETE == res) {
        return CONTINUE_ENUMERATION;
    }
    assert(ENUMERATION_INTERRUPTED == res);
    return STOP_ENUMERATION;
}

item_callback_result visit_map_entry_list(ast_visitor *v, toy_map_entry_list *map_entry_list)
{
    if (v->map_entry_list) {
        return v->map_entry_list(v, map_entry_list);
    }
    return default_map_entry_list(v, map_entry_list);
}

item_callback_result default_unary_op(ast_visitor *v, toy_unary_op *unary_op)
{
    return visit_expr(v, unary_op->arg);
}

item_callback_result visit_unary_op(ast_visitor *v, toy_unary_op *unary_op)
{
    if (v->unary_op) {
        return v->unary_op(v, unary_op);
    }
    return default_unary_op(v, unary_op);
}

item_callback_result default_postfix_decrement(ast_visitor *v, toy_postfix_decrement *postfix_decrement)
{
    return visit_identifier(v, &postfix_decrement->id);
}

item_callback_result visit_postfix_decrement(ast_visitor *v, toy_postfix_decrement *postfix_decrement)
{
    if (v->postfix_decrement) {
        return v->postfix_decrement(v, postfix_decrement);
    }
    return default_postfix_decrement(v, postfix_decrement);
}

item_callback_result default_postfix_increment(ast_visitor *v, toy_postfix_increment *postfix_increment)
{
    return visit_identifier(v, &postfix_increment->id);
}

item_callback_result visit_postfix_increment(ast_visitor *v, toy_postfix_increment *postfix_increment)
{
    if (v->postfix_increment) {
        return v->postfix_increment(v, postfix_increment);
    }
    return default_postfix_increment(v, postfix_increment);
}

item_callback_result default_prefix_decrement(ast_visitor *v, toy_prefix_decrement *prefix_decrement)
{
    return visit_identifier(v, &prefix_decrement->id);
}

item_callback_result visit_prefix_decrement(ast_visitor *v, toy_prefix_decrement *prefix_decrement)
{
    if (v->prefix_decrement) {
        return v->prefix_decrement(v, prefix_decrement);
    }
    return default_prefix_decrement(v, prefix_decrement);
}

item_callback_result default_prefix_increment(ast_visitor *v, toy_prefix_increment *prefix_increment)
{
    return visit_identifier(v, &prefix_increment->id);
}

item_callback_result visit_prefix_increment(ast_visitor *v, toy_prefix_increment *prefix_increment)
{
    if (v->prefix_increment) {
        return v->prefix_increment(v, prefix_increment);
    }
    return default_prefix_increment(v, prefix_increment);
}

item_callback_result default_ternary(ast_visitor *v, toy_ternary *ternary)
{
    item_callback_result res;
    res = visit_expr(v, ternary->condition);
    if (STOP_ENUMERATION == res) {
        return res;
    }
    res = visit_expr(v, ternary->if_true);
    if (STOP_ENUMERATION == res) {
        return res;
    }
    return visit_expr(v, ternary->if_false);
}

item_callback_result visit_ternary(ast_visitor *v, toy_ternary *ternary)
{
    if (v->ternary) {
        return v->ternary(v, ternary);
    }
    return default_ternary(v, ternary);
}

item_callback_result default_assignment(ast_visitor *v, toy_assignment *assignment)
{
    item_callback_result res;
    res = visit_identifier(v, &assignment->id);
    if (STOP_ENUMERATION == res) {
        return res;
    }
    return visit_expr(v, assignment->rhs);
}

item_callback_result visit_assignment(ast_visitor *v, toy_assignment *assignment)
{
    if (v->assignment) {
        return v->assignment(v, assignment);
    }
    return default_assignment(v, assignment);
}

item_callback_result default_collection_lookup(ast_visitor *v, toy_collection_lookup *lookup)
{
    item_callback_result res;
    res = visit_identifier(v, &lookup->id);
    if (STOP_ENUMERATION == res) {
        return res;
    }
    return visit_expr(v, lookup->rhs);
}

item_callback_result visit_collection_lookup(ast_visitor *v, toy_collection_lookup *lookup)
{
    if (v->collection_lookup) {
        return v->collection_lookup(v, lookup);
    }
    return default_collection_lookup(v, lookup);
}

item_callback_result default_method_call(ast_visitor *v, toy_method_call *method_call)
{
    item_callback_result res;
    res = visit_identifier(v, &method_call->id);
    if (STOP_ENUMERATION == res) {
        return res;
    }
    res = visit_string(v, method_call->method_name);
    if (STOP_ENUMERATION == res) {
        return res;
    }
    return visit_expr_list(v, method_call->args);
}

item_callback_result visit_method_call(ast_visitor *v, toy_method_call *method_call)
{
    if (v->method_call) {
        return v->method_call(v, method_call);
    }
    return default_method_call(v, method_call);
}

item_callback_result default_expr(ast_visitor *v, toy_expr *expr)
{
    switch (expr->type) {
    case EXPR_AND:
        return visit_binop(v, &expr->binary_op);
    case EXPR_ASSIGN:
        toy_assignment *assignment = &expr->assignment;
        return visit_assignment(v, assignment);
    case EXPR_COLLECTION_LOOKUP:
        toy_collection_lookup *lookup = &expr->collection_lookup;
        return visit_collection_lookup(v, lookup);
    case EXPR_COMMA:
    case EXPR_DIV:
    case EXPR_EQUAL:
    case EXPR_EXPONENT:
        return visit_binop(v, &expr->binary_op);
    case EXPR_FIELD_REF:
        toy_field_ref *field_ref = &expr->field_ref;
        return visit_field_ref(v, field_ref);
    case EXPR_FUNC_CALL:
        toy_func_call *call = &expr->func_call;
        return visit_func_call(v, call);
    case EXPR_GT:
    case EXPR_GTE:
        return visit_binop(v, &expr->binary_op);
    case EXPR_IDENTIFIER:
        toy_identifier *identifier = &expr->id;
        return visit_identifier(v, identifier);
    case EXPR_IN:
        return visit_binop(v, &expr->binary_op);
    case EXPR_LIST:
        return visit_expr_list(v, expr->expr_list);
    case EXPR_LITERAL:
        toy_val *val = expr->val;
        return visit_val(v, val);
    case EXPR_LT:
    case EXPR_LTE:
        return visit_binop(v, &expr->binary_op);
    case EXPR_MAP:
        toy_map_entry_list *entry_list = expr->map;
        return visit_map_entry_list(v, entry_list);
    case EXPR_METHOD_CALL:
        toy_method_call *method_call = &expr->method_call;
        return visit_method_call(v, method_call);
    case EXPR_MINUS:
    case EXPR_MODULUS:
    case EXPR_MUL:
    case EXPR_NEQUAL:
        return visit_binop(v, &expr->binary_op);
    case EXPR_NOT:
        return visit_unary_op(v, &expr->unary_op);
    case EXPR_OR:
    case EXPR_PLUS:
        return visit_binop(v, &expr->binary_op);
    case EXPR_POSTFIX_DECREMENT:
        return visit_postfix_decrement(v, &expr->postfix_decrement);
    case EXPR_POSTFIX_INCREMENT:
        return visit_postfix_increment(v, &expr->postfix_increment);
    case EXPR_PREFIX_DECREMENT:
        return visit_prefix_decrement(v, &expr->prefix_decrement);
    case EXPR_PREFIX_INCREMENT:
        return visit_prefix_increment(v, &expr->prefix_increment);
    case EXPR_TERNARY:
        return visit_ternary(v, &expr->ternary);
    case EXPR_UNEG:
        return visit_unary_op(v, &expr->unary_op);
    default:
        assert(0);
        break;
    }
}

item_callback_result visit_expr(ast_visitor *v, toy_expr *expr)
{
    if (v->expr) {
        return v->expr(v, expr);
    }
    return default_expr(v, expr);
}

item_callback_result default_for_stmt(ast_visitor *v, toy_for_stmt *for_stmt)
{
    item_callback_result res;
    res = visit_stmt(v, for_stmt->at_end);
    if (STOP_ENUMERATION == res) {
        return res;
    }
    res = visit_stmt(v, for_stmt->at_start);
    if (STOP_ENUMERATION == res) {
        return res;
    }
    res = visit_expr(v, for_stmt->condition);
    if (STOP_ENUMERATION == res) {
        return res;
    }
    return visit_block(v, for_stmt->body);
}

item_callback_result visit_for_stmt(ast_visitor *v, toy_for_stmt *for_stmt)
{
    if (v->for_stmt) {
        return v->for_stmt(v, for_stmt);
    }
    return default_for_stmt(v, for_stmt);
}

item_callback_result default_if_arm(ast_visitor *v, toy_if_arm *if_arm)
{
    item_callback_result res;
    res = visit_expr(v, if_arm->condition);
    if (STOP_ENUMERATION == res) {
        return res;
    }
    return visit_block(v, if_arm->code);
}

item_callback_result visit_if_arm(ast_visitor *v, toy_if_arm *if_arm)
{
    if (v->if_arm) {
        return v->if_arm(v, if_arm);
    }
    return default_if_arm(v, if_arm);
}

static item_callback_result if_arm_cb(ast_visitor *v, size_t index, toy_if_arm_list *item)
{
    toy_if_arm *if_arm = if_arm_list_payload(item);
    return visit_if_arm(v, if_arm);
}

item_callback_result default_if_arm_list(ast_visitor *v, toy_if_arm_list *if_arm_list)
{
    list_ast_visitor arm_visitor = { .list_vis.visit_entry = (list_entry_visit_func) if_arm_cb, .ast_vis = v };
    enumeration_result res = list_visitor_visit_list((list_visitor *) &arm_visitor, (generic_list *) if_arm_list);
    if (ENUMERATION_COMPLETE == res) {
        return CONTINUE_ENUMERATION;
    }
    assert(ENUMERATION_INTERRUPTED == res);
    return STOP_ENUMERATION;
}

item_callback_result visit_if_arm_list(ast_visitor *v, toy_if_arm_list *if_arm_list)
{
    if (v->if_arm_list) {
        return v->if_arm_list(v, if_arm_list);
    }
    return default_if_arm_list(v, if_arm_list);
}

item_callback_result default_if_stmt(ast_visitor *v, toy_if_stmt *if_stmt)
{
    item_callback_result res;
    res = visit_if_arm_list(v, if_stmt->arms);
    if (STOP_ENUMERATION == res) {
        return res;
    }
    return visit_block(v, if_stmt->elsepart);
}

item_callback_result visit_if_stmt(ast_visitor *v, toy_if_stmt *if_stmt)
{
    if (v->if_stmt) {
        return v->if_stmt(v, if_stmt);
    }
    return default_if_stmt(v, if_stmt);
}

item_callback_result default_null_stmt(ast_visitor *v)
{
    /* NOP */
    return CONTINUE_ENUMERATION;
}

item_callback_result visit_null_stmt(ast_visitor *v)
{
    if (v->null_stmt) {
        return v->null_stmt(v);
    }
    return default_null_stmt(v);
}

item_callback_result default_return_stmt(ast_visitor *v, toy_return_stmt *return_stmt)
{
    return visit_expr(v, return_stmt->expr);
}

item_callback_result visit_return_stmt(ast_visitor *v, toy_return_stmt *return_stmt)
{
    if (v->return_stmt) {
        return v->return_stmt(v, return_stmt);
    }
    return default_return_stmt(v, return_stmt);
}

item_callback_result default_var_decl(ast_visitor *v, toy_var_decl *var_decl)
{
    item_callback_result res;
    res = visit_string(v, var_decl->name);
    if (STOP_ENUMERATION == res) {
        return res;
    }
    return visit_expr(v, var_decl->value);
}

item_callback_result visit_var_decl(ast_visitor *v, toy_var_decl *var_decl)
{
    if (v->var_decl) {
        return v->var_decl(v, var_decl);
    }
    return default_var_decl(v, var_decl);
}

static item_callback_result var_decl_callback(list_ast_visitor *v, size_t index, toy_var_decl_list *item)
{
    toy_var_decl *var_decl = var_decl_list_payload(item);
    return visit_var_decl(v->ast_vis, var_decl);
}

item_callback_result default_var_decl_list(ast_visitor *v, toy_var_decl_list *var_decl_list)
{
    list_ast_visitor decl_visitor = {
        .list_vis.visit_entry = (list_entry_visit_func) var_decl_callback,
        .ast_vis = v
    };
    enumeration_result res = list_visitor_visit_list((list_visitor *) &decl_visitor, (generic_list *) var_decl_list);
    if (ENUMERATION_COMPLETE == res) {
        return CONTINUE_ENUMERATION;
    }
    assert(ENUMERATION_INTERRUPTED == res);
    return STOP_ENUMERATION;
}

item_callback_result visit_var_decl_list(ast_visitor *v, toy_var_decl_list *var_decl_list)
{
    if (v->var_decl_list) {
        return v->var_decl_list(v, var_decl_list);
    }
    return default_var_decl_list(v, var_decl_list);
}

item_callback_result default_while_stmt(ast_visitor *v, toy_while_stmt *while_stmt)
{
    item_callback_result res;
    res = visit_expr(v, while_stmt->condition);
    if (STOP_ENUMERATION == res) {
        return res;
    }
    return visit_block(v, while_stmt->body);
}

item_callback_result visit_while_stmt(ast_visitor *v, toy_while_stmt *while_stmt)
{
    if (v->while_stmt) {
        return v->while_stmt(v, while_stmt);
    }
    return default_while_stmt(v, while_stmt);
}

item_callback_result default_stmt(ast_visitor *v, toy_stmt *stmt)
{
    switch (stmt->type) {
    case STMT_BLOCK:
        toy_block *block = stmt->block_stmt.block;
        return visit_block(v, block);
    case STMT_BREAK:
        return visit_break_stmt(v);
    case STMT_CONTINUE:
        return visit_continue_stmt(v);
    case STMT_EXPR:
        toy_expr *expr = stmt->expr_stmt.expr;
        return visit_expr(v, expr);
    case STMT_FOR:
        toy_for_stmt *for_stmt = &stmt->for_stmt;
        return visit_for_stmt(v, for_stmt);
    case STMT_FUNC_DECL:
        toy_func_decl_stmt *fdecl = &stmt->func_decl_stmt;
        return visit_func_decl(v, fdecl);
    case STMT_IF:
        toy_if_stmt *if_stmt = &stmt->if_stmt;
        return visit_if_stmt(v, if_stmt);
    case STMT_NULL:
        return visit_null_stmt(v);
    case STMT_RETURN:
        toy_return_stmt *return_stmt = &stmt->return_stmt;
        return visit_return_stmt(v, return_stmt);
    case STMT_VAR_DECL:
        toy_var_decl_stmt *var_decl_stmt = &stmt->var_decl_stmt;
        toy_var_decl_list *var_decl_list = var_decl_stmt->var_decl_list;
        return visit_var_decl_list(v, var_decl_list);
    case STMT_WHILE:
        toy_while_stmt *while_stmt = &stmt->while_stmt;
        return visit_while_stmt(v, while_stmt);
    default:
        assert(0);
        break;
    }
}

item_callback_result visit_stmt(ast_visitor *v, toy_stmt *stmt)
{
    if (v->stmt) {
        return v->stmt(v, stmt);
    }
    return default_stmt(v, stmt);
}

static item_callback_result visit_stmt_callback(list_ast_visitor *v, size_t index, toy_stmt_list *item)
{
    toy_stmt *stmt = stmt_list_payload(item);
    return visit_stmt(v->ast_vis, stmt);
}

item_callback_result default_stmt_list(ast_visitor *v, toy_stmt_list *stmt_list)
{
    list_ast_visitor stmt_visitor = { .list_vis.visit_entry = (list_entry_visit_func) visit_stmt_callback, .ast_vis = v };
    enumeration_result res = list_visitor_visit_list((list_visitor *) &stmt_visitor, (generic_list *) stmt_list);
    if (ENUMERATION_COMPLETE == res) {
        return CONTINUE_ENUMERATION;
    }
    assert(ENUMERATION_INTERRUPTED == res);
    return STOP_ENUMERATION;
}

item_callback_result visit_stmt_list(ast_visitor *v, toy_stmt_list *stmt_list)
{
    if (v->stmt_list) {
        return v->stmt_list(v, stmt_list);
    }
    return default_stmt_list(v, stmt_list);
}

item_callback_result default_block(ast_visitor *v, toy_block *block)
{
    return visit_stmt_list(v, block->stmts);
}

item_callback_result visit_block(ast_visitor *v, toy_block *block)
{
    if (v->block) {
        return v->block(v, block);
    }
    return default_block(v, block);
}

item_callback_result default_func_expr(ast_visitor *v, toy_function *func)
{
    return visit_block(v, func->code);
}

item_callback_result visit_func_expr(ast_visitor *v, toy_function *func)
{
    if (v->func_expr) {
        return v->func_expr(v, func);
    }
    return default_func_expr(v, func);
}
