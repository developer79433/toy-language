#ifndef TOY_AST_H
#define TOY_AST_H 1

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "expr-types.h"
#include "toy-val-types.h"
#include "toy-str-list-types.h"
#include "function-types.h"
#include "toy-val-list-types.h"
#include "toy-map-entry-list.h"

typedef struct toy_var_decl_struct {
    toy_str name;
    toy_expr *value;
    struct toy_var_decl_struct *next;
} toy_var_decl;

typedef struct toy_func_decl_stmt_struct {
    toy_func_def def;
} toy_func_decl_stmt;

typedef struct toy_expr_stmt_struct {
    toy_expr *expr;
} toy_expr_stmt;

typedef struct toy_return_stmt_struct {
    toy_expr *expr;
} toy_return_stmt;

struct toy_if_arm_struct;
typedef struct toy_if_arm_struct {
    toy_expr *condition;
    toy_block code;
    struct toy_if_arm_struct *next;
} toy_if_arm;

typedef struct toy_if_stmt_struct {
    toy_if_arm *arms;
    toy_block elsepart;
} toy_if_stmt;

typedef struct toy_for_stmt_struct {
    toy_expr *condition;
    toy_stmt *at_start;
    toy_stmt *at_end;
    toy_block body;
} toy_for_stmt;

typedef struct toy_while_stmt_struct {
    toy_expr *condition;
    toy_block body;
} toy_while_stmt;

typedef struct toy_block_stmt_struct {
    toy_block block;
} toy_block_stmt;

/* TODO: Sort these alphabetically */
/* TODO: switch statement */
enum toy_stmt_type {
    STMT_BLOCK = 0,
    STMT_BREAK,
    STMT_CONTINUE,
    STMT_EXPR,
    STMT_FOR,
    STMT_FUNC_DECL,
    STMT_IF,
    STMT_NULL,
    STMT_RETURN,
    STMT_VAR_DECL,
    STMT_WHILE
};

struct toy_stmt_struct {
    enum toy_stmt_type type;
    struct toy_stmt_struct *next;
    union {
        toy_expr_stmt expr_stmt;
        toy_if_stmt if_stmt;
        toy_for_stmt for_stmt;
        toy_while_stmt while_stmt;
        toy_var_decl *var_decl_stmt;
        toy_func_decl_stmt func_decl_stmt;
        toy_return_stmt return_stmt;
        toy_block_stmt block_stmt;
    };
};

toy_stmt *alloc_stmt(enum toy_stmt_type type);
toy_if_arm *alloc_if_arm(toy_expr *condition, toy_block *code);
toy_var_decl *alloc_var_decl(toy_str name, toy_expr *value);
toy_stmt *append_stmt(toy_stmt *orig, toy_stmt *new);
toy_var_decl *append_var_decl(toy_var_decl *orig, toy_var_decl *new);
toy_if_arm *append_if_arm(toy_if_arm *orig, toy_if_arm *new);
const char *toy_val_type_name(enum toy_val_type val_type);
const char *toy_expr_type_name(enum toy_expr_type expr_type);
const char *toy_stmt_type_name(enum toy_stmt_type stmt_type);

#endif /* TOY_AST_H */
