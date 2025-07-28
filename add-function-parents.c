#include <assert.h>

#include "add-function-parents.h"
#include "expr-list.h"
#include "if-arm-list.h"
#include "stmt-list.h"
#include "var-decl-list.h"

static void add_parent_links_to_expr(toy_function *func, toy_expr *expr);

static void add_parent_links_to_binary_op(toy_function *func, toy_binary_op *binop)
{
    add_parent_links_to_expr(func, binop->arg1);
    add_parent_links_to_expr(func, binop->arg2);
}

typedef struct expr_cb_args_struct {
    toy_function *parent_func;
} expr_cb_args;

static item_callback_result expr_cb(void *cookie, size_t index, toy_expr_list *item)
{
    expr_cb_args *args = (expr_cb_args *) cookie;
    toy_expr *expr = expr_list_payload(item);
    add_parent_links_to_expr(args->parent_func, expr);
    return CONTINUE_ENUMERATION;
}

static void add_parent_links_to_expr_list(toy_function *func, toy_expr_list *expr_list)
{
    expr_cb_args args = { .parent_func = func };
    expr_list_foreach(expr_list, expr_cb, &args);
}

static void add_parent_links_to_val(toy_function *func, toy_val *val)
{
    if (val->type == VAL_FUNC) {
        toy_function *func_val = val->func;
        if (func_val->type == FUNC_USER_DECLARED) {
            func_val->parent = func;
            add_parent_links_to_stmt_list(func_val, func_val->code.stmts);
        }
    }
}

typedef struct map_entry_cb_args_struct {
    toy_function *parent_func;
} map_entry_cb_args;

static item_callback_result map_entry_cb(void *cookie, size_t index, toy_map_entry_list *item)
{
    map_entry_cb_args *args = (map_entry_cb_args *) cookie;
    toy_map_entry *map_entry = map_entry_list_payload(item);
    add_parent_links_to_expr(args->parent_func, map_entry->value);
    return CONTINUE_ENUMERATION;
}

static void add_parent_links_to_map_entry_list(toy_function *func, toy_map_entry_list *entry_list)
{
    map_entry_cb_args args = { .parent_func = func };
    map_entry_list_foreach(entry_list, map_entry_cb, &args);
}

static void add_parent_links_to_expr(toy_function *func, toy_expr *expr)
{
    switch (expr->type) {
    case EXPR_AND:
        add_parent_links_to_binary_op(func, &expr->binary_op);
        break;
    case EXPR_ASSIGN:
        add_parent_links_to_expr(func, expr->assignment.rhs);
        break;
    case EXPR_COLLECTION_LOOKUP:
        add_parent_links_to_expr(func, expr->collection_lookup.rhs);
        break;
    case EXPR_COMMA:
    case EXPR_DIV:
    case EXPR_EQUAL:
    case EXPR_EXPONENT:
        add_parent_links_to_binary_op(func, &expr->binary_op);
        break;
    case EXPR_FIELD_REF:
        break;
    case EXPR_FUNC_CALL:
        add_parent_links_to_expr_list(func, expr->func_call.args);
        break;
    case EXPR_GT:
    case EXPR_GTE:
        add_parent_links_to_binary_op(func, &expr->binary_op);
        break;
    case EXPR_IDENTIFIER:
        break;
    case EXPR_IN:
        add_parent_links_to_binary_op(func, &expr->binary_op);
        break;
    case EXPR_LIST:
        add_parent_links_to_expr_list(func, expr->list);
        break;
    case EXPR_LITERAL:
        toy_val *val = &expr->val;
        add_parent_links_to_val(func, val);
        break;
    case EXPR_LT:
    case EXPR_LTE:
        add_parent_links_to_binary_op(func, &expr->binary_op);
        break;
    case EXPR_MAP:
        toy_map_entry_list *entry_list = expr->map;
        add_parent_links_to_map_entry_list(func, entry_list);
        break;
    case EXPR_METHOD_CALL:
        toy_method_call *method_call = &expr->method_call;
        add_parent_links_to_expr_list(func, method_call->args);
        break;
    case EXPR_MINUS:
    case EXPR_MODULUS:
    case EXPR_MUL:
    case EXPR_NEQUAL:
        add_parent_links_to_binary_op(func, &expr->binary_op);
        break;
    case EXPR_NOT:
        add_parent_links_to_expr(func, expr->unary_op.arg);
        break;
    case EXPR_OR:
    case EXPR_PLUS:
        add_parent_links_to_binary_op(func, &expr->binary_op);
        break;
    case EXPR_POSTFIX_DECREMENT:
    case EXPR_POSTFIX_INCREMENT:
    case EXPR_PREFIX_DECREMENT:
    case EXPR_PREFIX_INCREMENT:
        add_parent_links_to_expr(func, expr->unary_op.arg);
        break;
    case EXPR_TERNARY:
        toy_ternary *ternary = &expr->ternary;
        add_parent_links_to_expr(func, ternary->condition);
        add_parent_links_to_expr(func, ternary->if_false);
        add_parent_links_to_expr(func, ternary->if_true);
        break;
    case EXPR_UNEG:
        add_parent_links_to_expr(func, expr->unary_op.arg);
        break;
    default:
        assert(0);
        break;
    }
}

typedef struct if_arm_cb_args_struct {
    toy_function *parent_func;
} if_arm_cb_args;

static item_callback_result if_arm_callback(void *cookie, size_t index, toy_if_arm_list *item)
{
    if_arm_cb_args *args = (if_arm_cb_args *) cookie;
    toy_if_arm *arm = if_arm_list_payload(item);
    add_parent_links_to_expr(args->parent_func, arm->condition);
    add_parent_links_to_stmt_list(args->parent_func, arm->code.stmts);
    return CONTINUE_ENUMERATION;
}

static void add_parent_links_to_if_arms(toy_function *func, toy_if_arm_list *if_arm_list)
{
    if_arm_cb_args args = {};
    if_arm_list_foreach(if_arm_list, if_arm_callback, &args);
}

typedef struct var_decl_args_struct {
    toy_function *parent_func;
} var_decl_args;

static item_callback_result vdecl_cb(void *cookie, size_t index, toy_var_decl_list *item)
{
    var_decl_args *args = (var_decl_args *) cookie;
    toy_var_decl *decl = var_decl_list_payload(item);
    toy_expr *ex = decl->value;
    if (ex->type == EXPR_LITERAL) {
        toy_val *val = &ex->val;
        add_parent_links_to_val(args->parent_func, val);
    }
    return CONTINUE_ENUMERATION;
}

static void add_parent_links_to_stmt(toy_function *function, toy_stmt *stmt)
{
    switch (stmt->type) {
    case STMT_BLOCK:
        toy_block *block = &stmt->block_stmt.block;
        add_parent_links_to_stmt_list(function, block->stmts);
        break;
    case STMT_BREAK:
    case STMT_CONTINUE:
        break;
    case STMT_EXPR:
        toy_expr *expr = stmt->expr_stmt.expr;
        add_parent_links_to_expr(function, expr);
        break;
    case STMT_FOR:
        toy_for_stmt *for_stmt = &stmt->for_stmt;
        add_parent_links_to_stmt(function, for_stmt->at_start);
        add_parent_links_to_stmt(function, for_stmt->at_end);
        add_parent_links_to_expr(function, for_stmt->condition);
        add_parent_links_to_stmt_list(function, for_stmt->body.stmts);
        break;
    case STMT_FUNC_DECL:
        toy_func_decl_stmt *fdecl = &stmt->func_decl_stmt;
        assert(fdecl->func.type == FUNC_USER_DECLARED);
        fdecl->func.parent = function;
        add_parent_links_to_stmt_list(&fdecl->func, fdecl->func.code.stmts);
        break;
    case STMT_IF:
        toy_if_stmt *if_stmt = &stmt->if_stmt;
        add_parent_links_to_if_arms(function, if_stmt->arms);
        add_parent_links_to_stmt_list(function, if_stmt->elsepart.stmts);
        break;
    case STMT_NULL:
        break;
    case STMT_RETURN:
        toy_return_stmt *return_stmt = &stmt->return_stmt;
        add_parent_links_to_expr(function, return_stmt->expr);
        break;
    case STMT_VAR_DECL:
        toy_var_decl_list *vdecl_list = &stmt->var_decl_stmt;
        var_decl_args vdecl_cb_args = { .parent_func = function };
        enumeration_result res = var_decl_list_foreach(vdecl_list, vdecl_cb, &vdecl_cb_args);
        assert(res == ENUMERATION_COMPLETE);
        break;
    case STMT_WHILE:
        toy_while_stmt *while_stmt = &stmt->while_stmt;
        add_parent_links_to_expr(function, while_stmt->condition);
        add_parent_links_to_stmt_list(function, while_stmt->body.stmts);
        break;
    default:
        assert(0);
        break;
    }
}

typedef struct stmt_cb_args_struct {
    toy_function *parent_func;
} stmt_cb_args;

static item_callback_result stmt_add_parent_func(void *cookie, size_t index, toy_stmt_list *item)
{
    stmt_cb_args *args = (stmt_cb_args *) cookie;
    toy_stmt *stmt = stmt_list_payload(item);
    add_parent_links_to_stmt(args->parent_func, stmt);
    return CONTINUE_ENUMERATION;
}

void add_parent_links_to_stmt_list(toy_function *function, toy_stmt_list *stmt_list)
{
    stmt_cb_args args = { .parent_func = function };
    stmt_list_foreach(stmt_list, stmt_add_parent_func, &args);
}
