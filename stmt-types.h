#ifndef TOY_STMT_TYPES_H
#define TOY_STMT_TYPES_H 1

#include "expr-types.h"
#include "val-types.h"
#include "str-list-types.h"
#include "function-types.h"
#include "val-list-types.h"
#include "map-entry-list.h"
#include "var-decl-types.h"
#include "var-decl-list-types.h"

typedef struct toy_func_decl_stmt_struct {
    toy_func_def def;
} toy_func_decl_stmt;

typedef struct toy_expr_stmt_struct {
    toy_expr *expr;
} toy_expr_stmt;

typedef struct toy_return_stmt_struct {
    toy_expr *expr;
} toy_return_stmt;

/* TODO: Should be named 'toy_if_arm_list' */
struct toy_if_arm_list_struct;
typedef struct toy_if_arm_list_struct {
    struct toy_if_arm_list_struct *next;
    toy_expr *condition;
    toy_block code;
} toy_if_arm_list;

typedef struct toy_if_stmt_struct {
    toy_if_arm_list *arms;
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
        toy_var_decl_list *var_decl_stmt;
        toy_func_decl_stmt func_decl_stmt;
        toy_return_stmt return_stmt;
        toy_block_stmt block_stmt;
    };
};

#endif /* TOY_STMT_TYPES_H */
