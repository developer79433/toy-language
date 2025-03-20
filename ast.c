#include <string.h>
#include <malloc.h>

#include "ast.h"

void fatal_error(const char *fmt, ...)
{
    va_list argptr;
    va_start(argptr, fmt);
    vfprintf(stderr, fmt, argptr);
    va_end(argptr);
    abort();
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

#define mymalloc(the_type) ((the_type *) malloc(sizeof(the_type)))

toy_stmt *alloc_stmt(enum toy_stmt_type type)
{
    toy_stmt *stmt;
    stmt = mymalloc(toy_stmt);
    stmt->type = type;
    return stmt;
}

toy_if_arm *alloc_if_arm(toy_expr *condition, toy_stmt *code)
{
    toy_if_arm *arm;
    arm = mymalloc(toy_if_arm);
    arm->condition = condition;
    arm->code = code;
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

toy_var_decl *alloc_var_decl()
{
    toy_var_decl *var_decl;
    var_decl = mymalloc(toy_var_decl);
    var_decl->next = NULL;
    var_decl->name = NULL;
    var_decl->value = NULL;
    return var_decl;
}

toy_expr *alloc_expr(enum toy_expr_type type)
{
    toy_expr *expr;
    expr = mymalloc(toy_expr);
    expr->type = type;
    return expr;
}

toy_expr *alloc_expr_func_decl(void *formalparams, toy_stmt *code)
{
    toy_expr *expr;
    expr = (toy_expr *) malloc(sizeof(toy_expr) + sizeof(toy_func_expr));
    expr->func_decl = (toy_func_expr *) (expr + 1);
    expr->func_decl->def.name = ""; /* TODO: generated unique name */
    expr->func_decl->def.param_names = formalparams;
    expr->func_decl->def.code = code;
    return expr;
}

toy_list *alloc_toy_list(toy_expr *first_elem)
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

static const char *toy_expr_type_names[] = {
    "logical and",
    "assignment",
    "boolean",
    "division",
    "equal to",
    "function call",
    "function declaration"
    "greater than",
    "greater than or equal to",
    "identifier",
    "in list",
    "list",
    "less than",
    "less than or equal to",
    "map",
    "subtraction",
    "multiplication",
    "not equal to",
    "logical inverse",
    "numeric",
    "logical or",
    "addition",
    "string",
    "unary negation"
};

const char *toy_expr_type_name(enum toy_expr_type expr_type)
{
    return toy_expr_type_names[expr_type];
}

static const char *toy_stmt_type_names[] = {
    "expression statement",
    "for loop",
    "function declaration",
    "null statement",
    "if statement",
    "variable declaration",
    "while loop"
};

const char *toy_stmt_type_name(enum toy_stmt_type stmt_type)
{
    return toy_stmt_type_names[stmt_type];
}

static void dump_list(FILE *f, toy_list *list)
{
    fputs("[ ", f);
    for (toy_list *cur = list; cur; cur = cur->next) {
        dump_expr(f, cur->expr);
    }
    fputs("]\n", f);
}

static void dump_map(FILE *f, toy_map *map)
{
    /* TODO */
}

void dump_expr(FILE *f, const toy_expr *expr) {
    fprintf(f, "\"%s\" {\n", toy_expr_type_name(expr->type));
    switch (expr->type) {
    case EXPR_AND:
    case EXPR_GT:
    case EXPR_GTE:
    case EXPR_DIV:
    case EXPR_IN:
    case EXPR_LT:
    case EXPR_LTE:
    case EXPR_MINUS:
    case EXPR_OR:
    case EXPR_MUL:
    case EXPR_NEQUAL:
    case EXPR_PLUS:
    case EXPR_EQUAL:
    case EXPR_ASSIGN:
        fputs("Operand 1 {\n", f);
        dump_expr(f, expr->binary_op->arg1);
        fputs("}\nOperand 2 {\n", f);
        dump_expr(f, expr->binary_op->arg2);
        fputs("}\n", f);
        break;
    case EXPR_BOOL:
        fputs(expr->bool ? "True" : "False", f);
        break;
    case EXPR_FUNC_CALL:
        fprintf(f, "%s(", expr->func_call->func_name);
        toy_list *arg;
        for (arg = expr->func_call->args; arg->next; arg = arg->next) {
            dump_expr(f, arg->expr);
            fputs(", ", f);
        }
        dump_expr(f, arg->expr);
        fputs(")\n", f);
        break;
    case EXPR_FUNC_DECL:
        fprintf(f, "fun %s(", expr->func_call->func_name);
        toy_list *cur;
        for (cur = expr->func_call->args; cur->next; cur = cur->next) {
            dump_expr(f, cur->expr);
            fputs(", ", f);
        }
        dump_expr(f, cur->expr);
        fputs(")\n", f);
        break;
    case EXPR_IDENTIFIER:
        fprintf(f, "\"%s\"", expr->str);
        break;
    case EXPR_LIST:
        dump_list(f, expr->list);
        break;
    case EXPR_MAP:
        dump_map(f, expr->map);
        break;
    case EXPR_NOT:
        fputs("not ", f);
        dump_expr(f, expr->unary_op->arg);
        break;
    case EXPR_NUM:
        fprintf(f, "\"%f\"", expr->num);
        break;
    case EXPR_STR:
        fprintf(f, "\"%s\"", expr->str);
        break;
    case EXPR_UNEG:
        fputs("-", f);
        dump_expr(f, expr->unary_op->arg);
        break;
    default:
        invalid_expr_type(expr->type);
        break;
    }
    fputs("}\n", f);
}

void dump_stmt(FILE *f, const toy_stmt *stmt)
{
    fprintf(f, "\"%s\" {\n}", toy_stmt_type_name(stmt->type));
    switch (stmt->type) {
    case STMT_EXPR:
        dump_expr(f, stmt->expr_stmt.expr);
        break;
    case STMT_FOR:
        break;
    case STMT_FUNC_DECL:
        break;
    case STMT_IF:
        {
            for (toy_if_arm *arm = stmt->if_stmt.arms; arm; arm = arm->next) {
                fputs("elseif (\n", f);
                dump_expr(f, arm->condition);
                fputs(") {\n", f);
                dump_stmts(f, arm->code);
                fputs("}\n", f);
            }
            fputs("else (\n", f);
            dump_stmts(f, stmt->if_stmt.elsepart);
            fputs("}\n", f);
        }
        break;
    case STMT_NULL:
        break;
    case STMT_VAR_DECL:
        /* TODO */
        break;
    case STMT_WHILE:
        break;
    default:
        invalid_stmt_type(stmt->type);
        break;
    }
    fputs("}\n", f);
}

void dump_stmts(FILE *f, const toy_stmt *stmts)
{
    for (const toy_stmt *s = stmts; s; s = s->next) {
        dump_stmt(f, s);
    }
}
