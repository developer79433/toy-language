#ifndef TOY_EXPR_TYPES_H
#define TOY_EXPR_TYPES_H 1

#include "str-types.h"
#include "val-types.h"
#include "map-expr-entry-list-types.h"
#include "expr-list-types.h"
#include "constant-types.h"

struct toy_expr_struct;
typedef struct toy_expr_struct toy_expr;

enum toy_expr_type_enum {
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

#define EXPR_MAX EXPR_UNEG

typedef enum toy_expr_type_enum toy_expr_type;

/* TODO: Belongs elsewhere */

typedef enum reference_type_enum {
    REF_UNDEFINED    = 0,
    REF_FUNC_DECL    = 1,
    REF_FUNC_PARAM   = 2,
    REF_VAR_DECL     = 3,
    REF_PREDEF_CONST = 4,
    REF_PREDEF_FUNC  = 5
} reference_type;

struct toy_func_decl_stmt_struct;
typedef struct toy_func_decl_stmt_struct toy_func_decl_stmt;

struct toy_var_decl_struct;
typedef struct toy_var_decl_struct toy_var_decl;

typedef struct closure_struct {
    size_t frames_up;
    size_t var_index;
} closure;

typedef struct resolved_name_struct {
    reference_type type;
    union {
        /* function parameter */
        closure func_param;
        /* variable declaration */
        closure var_decl;
        /* function declaration */
        toy_func_decl_stmt *func_decl_stmt;
        /* TODO: Can this be a toy_val too? */
        const predefined_constant *predef_const;
        const toy_val *predef_func;
    };
} resolved_name;

typedef struct toy_identifier_struct {
    toy_str name;
    /* TODO: This is leakage of the name resolution phase into the parse tree */
    resolved_name resolved;
} toy_identifier;

typedef struct toy_unary_op_struct {
    toy_expr *arg;
} toy_unary_op;

typedef struct toy_binary_op_struct {
    toy_expr *arg1;
    toy_expr *arg2;
} toy_binary_op;

/* TODO: Could be merged with toy_method_call */
typedef struct toy_func_call_struct {
    toy_identifier id;
    toy_expr_list *args;
} toy_func_call;

typedef struct toy_assignment_struct {
    toy_identifier id;
    toy_expr *rhs;
} toy_assignment;

typedef struct toy_postfix_decrement_struct {
    toy_identifier id;
} toy_postfix_decrement;

typedef struct toy_postfix_increment_struct {
    toy_identifier id;
} toy_postfix_increment;

typedef struct toy_prefix_decrement_struct {
    toy_identifier id;
} toy_prefix_decrement;

typedef struct toy_prefix_increment_struct {
    toy_identifier id;
} toy_prefix_increment;

typedef struct toy_ternary_struct {
    toy_expr *condition;
    toy_expr *if_true;
    toy_expr *if_false;
} toy_ternary;

typedef struct toy_field_ref_struct {
    toy_identifier id;
    toy_str field_name;
} toy_field_ref;

typedef struct toy_method_call_struct {
    toy_identifier id;
    toy_str method_name;
    toy_expr_list *args;
} toy_method_call;

typedef struct toy_collection_lookup_struct {
    toy_identifier id;
    toy_expr *rhs;
} toy_collection_lookup;

struct toy_expr_struct {
    toy_expr_type type;
    union {
        /* map and list not used, since those contain expressions, handled below */
        toy_val *val;
        /* Function call: str(expr1, expr2) */
        toy_func_call func_call;
        /* Object method call: expr1.str(expr2, expr3) */
        toy_method_call method_call;
        // Unary operation: !expr
        toy_unary_op unary_op;
        // Binary operation: expr1 + expr2
        toy_binary_op binary_op;
        // Assignment operation: identifier = expr
        toy_assignment assignment;
        // Postfix decrement operation: identifier--
        toy_postfix_decrement postfix_decrement;
        // Postfix increment operation: identifier++
        toy_postfix_increment postfix_increment;
        // Prefix decrement operation: --identifier
        toy_prefix_decrement prefix_decrement;
        // Prefix increment operation: ++identifier
        toy_prefix_increment prefix_increment;
        // Ternary operation: expr1 ? expr2 : expr3
        toy_ternary ternary;
        // Object field lookup: expr1.field_name
        toy_field_ref field_ref;
        // Collection lookup: expr1[expr2]
        toy_collection_lookup collection_lookup;
        // Identifier: str
        toy_identifier id;
        // Map expression: { str1: expr1, str2: expr2 }
        toy_map_expr_entry_list *map;
        // List expression: [ expr1, expr2 ]
        toy_expr_list *expr_list;
    };
};

#endif /* TOY_EXPR_TYPES_H */
