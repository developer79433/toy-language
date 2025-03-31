#include <string.h>

#include "mymalloc.h"
#include "ast.h"
#include "list.h"
#include "map.h"
#include "dump.h"

static const char *toy_expr_type_names[] = {
    "logical and",
    "assignment",
    "boolean",
    "comma",
    "division",
    "equal to",
    "exponentiation",
    "field reference",
    "function call",
    "function declaration",
    "greater than",
    "greater than or equal to",
    "identifier",
    "in list",
    "list",
    "less than",
    "less than or equal to",
    "map",
    "subtraction",
    "modulus",
    "multiplication",
    "not equal to",
    "logical negation",
    "null",
    "numeric",
    "logical or",
    "addition",
    "postfix decrement",
    "postfix increment",
    "prefix decrement",
    "prefix increment",
    "string",
    "unary negation"
};

void fatal_error(const char *fmt, ...)
{
    va_list argptr;
    va_start(argptr, fmt);
    vfprintf(stderr, fmt, argptr);
    va_end(argptr);
    exit(1);
}

void invalid_operand(enum toy_expr_type expr_type, const toy_expr *operand)
{
    dump_expr(stderr, operand);
    fatal_error("Invalid operand for expression %s", toy_expr_type_name(expr_type));
}

void invalid_expr_type(enum toy_expr_type expr_type) {
    fatal_error("Invalid expression type %s", toy_expr_type_name(expr_type));
}

void invalid_stmt_type(enum toy_stmt_type stmt_type)
{
    fatal_error("Invalid statement type %s", toy_stmt_type_name(stmt_type));
}

void invalid_cast(enum toy_expr_type expr_type, const toy_expr *expr)
{
    dump_expr(stderr, expr);
    fatal_error("Cannot convert to %s", toy_expr_type_name(expr_type));
}

void duplicate_identifier(toy_str name)
{
    fatal_error("Attempt to redeclare identifier '%s'", name);
}

void undeclared_identifier(toy_str name)
{
    fatal_error("Undeclared identifier '%s'", name);
}

void readonly_identifier(toy_str name)
{
    fatal_error("Read-only identifier '%s'", name);
}

void invalid_list_index(toy_list *list, toy_num index)
{
    dump_list(stderr, list);
    fatal_error("Invalid list index %d", index);
}

void too_many_arguments(toy_num expected, toy_list *args)
{
    dump_list(stderr, args);
    fatal_error("Too many arguments: expected %d, received %d", expected, list_len(args));
}

toy_map_entry *alloc_map_entry(toy_expr *key, toy_expr *value)
{
    toy_map_entry *map_entry;
    map_entry = mymalloc(toy_map_entry);
    map_entry->key = key;
    map_entry->value = value;
    map_entry->next = NULL;
    return map_entry;
}

toy_stmt *alloc_stmt(enum toy_stmt_type type)
{
    toy_stmt *stmt;
    stmt = mymalloc(toy_stmt);
    stmt->type = type;
    return stmt;
}

toy_if_arm *alloc_if_arm(toy_expr *condition, toy_block *block)
{
    toy_if_arm *arm;
    arm = mymalloc(toy_if_arm);
    arm->condition = condition;
    arm->code.stmts = block->stmts;
    arm->next = NULL;
    return arm;
}

toy_str_list *alloc_str_list(const char *str)
{
    size_t size = sizeof(toy_str_list) + strlen(str) + 1;
    void *mem = malloc(size);
    toy_str_list *list = (toy_str_list *) mem;
    list->next = NULL;
    list->str = (char *) (list + 1);
    strcpy(list->str, str);
    return list;
}

toy_var_decl *alloc_var_decl(toy_str name, toy_expr *value)
{
    toy_var_decl *var_decl;
    var_decl = mymalloc(toy_var_decl);
    var_decl->name = name;
    var_decl->value = value;
    var_decl->next = NULL;
    return var_decl;
}

toy_expr *alloc_unary_op_expr(enum toy_expr_type expr_type)
{
    toy_expr *expr;
    expr = mymalloc(toy_expr);
    expr->type = expr_type;
    expr->unary_op.arg = NULL;
    return expr;
}

toy_expr *alloc_binary_op_expr(enum toy_expr_type expr_type)
{
    toy_expr *expr;
    expr = mymalloc(toy_expr);
    expr->type = expr_type;
    expr->binary_op.arg1 = NULL;
    expr->binary_op.arg2 = NULL;
    return expr;
}

toy_expr *alloc_expr(enum toy_expr_type type)
{
    toy_expr *expr;
    expr = mymalloc(toy_expr);
    expr->type = type;
    return expr;
}

toy_expr *alloc_expr_func_decl(toy_str_list *formalparams, toy_block *body)
{
    toy_expr *expr;
    expr = mymalloc(toy_expr);
    expr->type = EXPR_FUNC_DECL;
    expr->func_decl.def.name = ""; /* TODO: generated unique name */
    expr->func_decl.def.param_names = formalparams;
    expr->func_decl.def.code.stmts = body->stmts;
    return expr;
}

toy_list *alloc_list(toy_expr *first_elem)
{
    toy_list *list;
    list = mymalloc(toy_list);
    list->expr = first_elem;
    list->next = NULL;
    return list;
}

toy_stmt *append_stmt(toy_stmt *orig, toy_stmt *new)
{
    toy_stmt *tmp = orig;
    while (tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = new;
    return orig;
}

toy_var_decl *append_var_decl(toy_var_decl *orig, toy_var_decl *new)
{
    toy_var_decl *tmp = orig;
    while (tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = new;
    return orig;
}

toy_if_arm *append_if_arm(toy_if_arm *orig, toy_if_arm *new)
{
    toy_if_arm *tmp = orig;
    while (tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = new;
    return orig;
}

toy_str_list *append_str_list(toy_str_list *orig, toy_str_list *new)
{
    toy_str_list *tmp = orig;
    while (tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = new;
    return orig;
}

toy_list *append_list(toy_list *orig, toy_list *new)
{
    toy_list *tmp = orig;
    while (tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = new;
    return orig;
}

toy_map_entry *append_map_entry(toy_map_entry *orig, toy_map_entry *new)
{
    toy_map_entry *tmp = orig;
    while (tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = new;
    return orig;
}

const char *toy_expr_type_name(enum toy_expr_type expr_type)
{
    return toy_expr_type_names[expr_type];
}

static const char *toy_stmt_type_names[] = {
    "block statement",
    "break statement",
    "continue statement",
    "expression statement",
    "for loop",
    "function declaration",
    "if statement",
    "null statement",
    "return statement",
    "variable declaration",
    "while loop"
};

const char *toy_stmt_type_name(enum toy_stmt_type stmt_type)
{
    return toy_stmt_type_names[stmt_type];
}
