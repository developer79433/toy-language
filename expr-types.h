#ifndef TOY_EXPR_TYPES_H
#define TOY_EXPR_TYPES_H 1

#include "str-types.h"
#include "val-types.h"
#include "map-entry-list-types.h"
#include "expr-list-types.h"

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

struct toy_expr_struct {
    toy_expr_type type;
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

#endif /* TOY_EXPR_TYPES_H */
