#ifndef TOY_AST_H
#define TOY_AST_H 1

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "toy-val-types.h"
#include "toy-str-list-types.h"
#include "function-types.h"
#include "toy-val-list-types.h"
#include "toy-expr-list.h"
#include "toy-map-entry-list.h"

struct toy_expr_struct;
typedef struct toy_expr_struct toy_expr;

typedef struct toy_unary_op_struct {
    toy_expr *arg;
} toy_unary_op;

typedef struct toy_binary_op_struct {
    toy_expr *arg1;
    toy_expr *arg2;
} toy_binary_op;

/* TODO: Could be merged with toy_method_call */
typedef struct toy_func_call_struct {
    toy_str func_name;
    toy_expr_list *args;
} toy_func_call;

typedef struct toy_assignment_struct {
    toy_str lhs;
    toy_expr *rhs;
} toy_assignment;

typedef struct toy_postfix_decrement_struct {
    toy_str id;
} toy_postfix_decrement;

typedef struct toy_postfix_increment_struct {
    toy_str id;
} toy_postfix_increment;

typedef struct toy_prefix_decrement_struct {
    toy_str id;
} toy_prefix_decrement;

typedef struct toy_prefix_increment_struct {
    toy_str id;
} toy_prefix_increment;

typedef struct toy_ternary_struct {
    toy_expr *condition;
    toy_expr *if_true;
    toy_expr *if_false;
} toy_ternary;

typedef struct toy_field_ref_struct {
    toy_str lhs;
    toy_str rhs;
} toy_field_ref;

typedef struct toy_method_call_struct {
    toy_str target;
    toy_str method_name;
    toy_expr_list *args;
} toy_method_call;

typedef struct toy_collection_lookup_struct {
    toy_str lhs;
    toy_expr *rhs;
} toy_collection_lookup;

enum toy_expr_type {
    EXPR_AND = 0,
    EXPR_ASSIGN,
    EXPR_COLLECTION_LOOKUP,
    EXPR_COMMA,
    EXPR_DIV,
    EXPR_EQUAL,
    EXPR_EXPONENT,
    EXPR_FIELD_REF,
    EXPR_FUNC_CALL,
    EXPR_GT,
    EXPR_GTE,
    EXPR_IDENTIFIER,
    EXPR_IN,
    EXPR_LIST,
    EXPR_LITERAL,
    EXPR_LT,
    EXPR_LTE,
    EXPR_MAP,
    EXPR_METHOD_CALL,
    EXPR_MINUS,
    EXPR_MODULUS,
    EXPR_MUL,
    EXPR_NEQUAL,
    EXPR_NOT,
    EXPR_OR,
    EXPR_PLUS,
    EXPR_POSTFIX_DECREMENT,
    EXPR_POSTFIX_INCREMENT,
    EXPR_PREFIX_DECREMENT,
    EXPR_PREFIX_INCREMENT,
    EXPR_TERNARY,
    EXPR_UNEG
};

struct toy_expr_struct {
    enum toy_expr_type type;
    union {
        toy_val val;
        toy_func_call func_call;
        toy_method_call method_call;
        toy_unary_op unary_op;
        toy_binary_op binary_op;
        toy_assignment assignment;
        toy_postfix_decrement postfix_decrement;
        toy_postfix_increment postfix_increment;
        toy_prefix_decrement prefix_decrement;
        toy_prefix_increment prefix_increment;
        toy_ternary ternary;
        toy_field_ref field_ref;
        toy_collection_lookup collection_lookup;
        /* These two hold AST expressions, as opposed to the interpreter values held in the toy_val member. */
        /* TODO: Do I really need these? */
        toy_map_entry_list *map;
        toy_expr_list *list;
    };
};

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
toy_expr *alloc_unary_op_expr(enum toy_expr_type expr_type);
toy_expr *alloc_binary_op_expr(enum toy_expr_type expr_type);
toy_expr *alloc_expr(enum toy_expr_type type);
toy_expr *alloc_expr_literal(enum toy_val_type val_type);
toy_expr *alloc_expr_func_decl(toy_str_list *formalparams, toy_block *body);
toy_stmt *append_stmt(toy_stmt *orig, toy_stmt *new);
toy_var_decl *append_var_decl(toy_var_decl *orig, toy_var_decl *new);
toy_if_arm *append_if_arm(toy_if_arm *orig, toy_if_arm *new);
const char *toy_val_type_name(enum toy_val_type val_type);
const char *toy_expr_type_name(enum toy_expr_type expr_type);
const char *toy_stmt_type_name(enum toy_stmt_type stmt_type);

#endif /* TOY_AST_H */
