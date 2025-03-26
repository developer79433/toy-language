#ifndef TOY_AST_H
#define TOY_AST_H 1

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

typedef unsigned int toy_bool;
typedef double toy_num;
typedef char *toy_str;

struct toy_expr_struct;
struct toy_stmt_struct;
typedef struct toy_expr_struct toy_expr;
typedef struct toy_stmt_struct toy_stmt;

typedef struct symbol_table_struct {
    /* TODO */
} symbol_table;

typedef struct toy_block_struct {
    toy_stmt *stmts;
    symbol_table symbols;
} toy_block;

typedef struct toy_list_struct {
    toy_expr *expr;
    struct toy_list_struct *next;
} toy_list;

typedef struct toy_map_entry_struct {
    toy_expr *key;
    toy_expr *value;
    struct toy_map_entry_struct *next;
} toy_map_entry;

/* TODO: dynamic resizing */
#define NUM_BUCKETS 13

typedef struct toy_map_struct {
    /**
     * TODO: Move these out of the parser and into the interpreter.
     * The buckets etc are a toy runtime concern - they are irrelevant to parsing.
     * But at the moment there is no distinction betweent he AST's structures and
     * those used by the interpreter, so nowhere to move this to.
     */
    size_t num_items;
    toy_map_entry *buckets[NUM_BUCKETS];
} toy_map;

typedef struct toy_str_list_struct {
    toy_str str;
    struct toy_str_list_struct *next;
} toy_str_list;

typedef struct toy_func_def_struct {
    toy_str name;
    toy_str_list *param_names;
    toy_block code;
} toy_func_def;

typedef struct toy_func_expr_struct {
    toy_func_def def;
} toy_func_expr;

typedef struct toy_unary_op_struct {
    toy_expr *arg;
} toy_unary_op;

typedef struct toy_binary_op_struct {
    toy_expr *arg1;
    toy_expr *arg2;
} toy_binary_op;

typedef struct toy_func_call_struct {
    toy_str func_name;
    toy_list *args;
} toy_func_call;

typedef struct toy_assignment_struct {
    toy_str lhs;
    toy_expr *rhs;
} toy_assignment;

enum toy_expr_type {
    EXPR_AND = 0,
    EXPR_ASSIGN,
    EXPR_BOOL,
    EXPR_COMMA,
    EXPR_DIV,
    EXPR_EQUAL,
    EXPR_FUNC_CALL,
    EXPR_FUNC_DECL,
    EXPR_GT,
    EXPR_GTE,
    EXPR_IDENTIFIER,
    EXPR_IN,
    EXPR_LIST,
    EXPR_LT,
    EXPR_LTE,
    EXPR_MAP,
    EXPR_MINUS,
    EXPR_MUL,
    EXPR_NEQUAL,
    EXPR_NOT,
    EXPR_NUM,
    EXPR_OR,
    EXPR_PLUS,
    EXPR_STR,
    EXPR_UNEG
};

struct toy_expr_struct {
    enum toy_expr_type type;
    union {
        toy_bool bool;
        toy_num num;
        toy_str str;
        toy_list *list;
        toy_map *map;
        toy_func_call func_call;
        toy_unary_op unary_op;
        toy_binary_op binary_op;
        toy_func_expr func_decl;
        toy_assignment assignment;
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

enum toy_stmt_type {
    STMT_EXPR = 0,
    STMT_FOR,
    STMT_FUNC_DECL,
    STMT_NULL,
    STMT_IF,
    STMT_VAR_DECL,
    STMT_WHILE,
    STMT_RETURN
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
    };
};

void fatal_error(const char *fmt, ...);
void invalid_operand(enum toy_expr_type expr_type, const toy_expr *operand);
void invalid_expr_type(enum toy_expr_type expr_type);
void invalid_stmt_type(enum toy_stmt_type stmt_type);
void invalid_cast(enum toy_expr_type expr_type, const toy_expr *expr);
void dump_str(FILE *f, const toy_str str);
void dump_identifier(FILE *f, const toy_str str);
void dump_identifier_list(FILE *f, const toy_str_list *list);
void dump_list(FILE *f, toy_list *list);
void dump_expr(FILE *f, const toy_expr *expr);
void dump_stmts(FILE *f, const toy_stmt *stmts);
void dump_stmt(FILE *f, const toy_stmt *stmts);
toy_stmt *alloc_stmt(enum toy_stmt_type type);
toy_map_entry *alloc_map_entry(toy_expr *key, toy_expr *value);
toy_if_arm *alloc_if_arm(toy_expr *condition, toy_block *code);
toy_str_list *alloc_str_list(const char * str);
toy_list *alloc_list(toy_expr *first_elem);
toy_var_decl *alloc_var_decl(toy_str name, toy_expr *value);
toy_expr *alloc_unary_op_expr(enum toy_expr_type expr_type);
toy_expr *alloc_binary_op_expr(enum toy_expr_type expr_type);
toy_expr *alloc_expr(enum toy_expr_type type);
toy_expr *alloc_expr_func_decl(toy_str_list *formalparams, toy_block *body);
toy_stmt *append_stmt(toy_stmt *orig, toy_stmt *new);
toy_var_decl *append_var_decl(toy_var_decl *orig, toy_var_decl *new);
toy_if_arm *append_if_arm(toy_if_arm *orig, toy_if_arm *new);
toy_str_list *append_str_list(toy_str_list *orig, toy_str_list *new);
toy_list *append_list(toy_list *orig, toy_list *new);
toy_map_entry *append_map_entry(toy_map_entry *orig, toy_map_entry *new);
const char *toy_expr_type_name(enum toy_expr_type expr_type);
const char *toy_stmt_type_name(enum toy_stmt_type stmt_type);

#endif /* TOY_AST_H */
