#include <assert.h>
#include <string.h>

#include "expr.h"
#include "mymalloc.h"
#include "val.h"
#include "map-val.h"
#include "function.h"
#include "constants.h"
#include "predef-function.h"
#include "errors.h"
#include "str.h"
#include "str-list.h"
#include "stmt.h"
#include "stmt-list.h"
#include "var-decl-list.h"
#include "if-arm-list.h"
#include "expr-list.h"
#include "log.h"

static const char *toy_expr_type_names[] = {
    "logical and",
    "assignment",
    "collection lookup",
    "comma",
    "division",
    "equal to",
    "exponentiation",
    "field reference",
    "function call",
    "greater than",
    "greater than or equal to",
    "identifier",
    "in list",
    "list",
    "literal",
    "less than",
    "less than or equal to",
    "object",
    "method call",
    "subtraction",
    "modulus",
    "multiplication",
    "not equal to",
    "logical negation",
    "logical or",
    "addition",
    "postfix decrement",
    "postfix increment",
    "prefix decrement",
    "prefix increment",
    "unary negation"
};

const char *toy_expr_type_name(toy_expr_type expr_type)
{
    return toy_expr_type_names[expr_type];
}

toy_expr *alloc_unary_op_expr(toy_expr_type expr_type)
{
    toy_expr *expr;
    expr = mymalloc(toy_expr);
    expr->type = expr_type;
    expr->unary_op.arg = NULL;
    return expr;
}

toy_expr *alloc_binary_op_expr(toy_expr_type expr_type)
{
    toy_expr *expr;
    expr = mymalloc(toy_expr);
    expr->type = expr_type;
    expr->binary_op.arg1 = NULL;
    expr->binary_op.arg2 = NULL;
    return expr;
}

toy_expr *alloc_expr(toy_expr_type expr_type)
{
    toy_expr *expr;
    assert(EXPR_LITERAL != expr_type);
    expr = mymalloc(toy_expr);
    expr->type = expr_type;
    return expr;
}

toy_expr *alloc_expr_literal(toy_val_type val_type)
{
    toy_expr *expr = malloc(sizeof(toy_expr) + sizeof(toy_val));
    expr->type = EXPR_LITERAL;
    expr->val = (toy_val *) (expr + 1);
    expr->val->type = val_type;
    switch (val_type) {
    case VAL_BOOL:
    case VAL_FUNC:
        break;
    case VAL_LIST:
    case VAL_MAP:
        /* These should have been handled as a list or map of expressions, not of values */
        assert(0);
        break;
    case VAL_NULL:
    case VAL_NUM:
    case VAL_STR:
        break;
    default:
        assert(0);
        break;
    }
    return expr;
}

toy_expr *alloc_expr_func_decl(toy_str_list *formalparams, toy_block *block)
{
    toy_expr *expr;
    expr = (toy_expr *) malloc(sizeof(toy_expr) + sizeof(toy_val) + sizeof(toy_function));
    expr->type = EXPR_LITERAL;
    expr->val = (toy_val *) (expr + 1);
    expr->val->type = VAL_FUNC;
    expr->val->func = (toy_function *) (expr->val + 1);
    expr->val->func->type = FUNC_USER_DECLARED;
    expr->val->func->name = ""; /* TODO: generated unique name */
    expr->val->func->code->stmts = block->stmts;
    expr->val->func->param_names = formalparams;
    return expr;
}

toy_expr *alloc_expr_func_call(toy_str id, toy_expr_list *args)
{
    toy_expr *expr;
    expr = mymalloc(toy_expr);
    expr->type = EXPR_FUNC_CALL;
    expr->func_call.id.name = id;
    expr->func_call.args = args;
    return expr;
}

toy_expr *alloc_expr_method_call(toy_str lhs, toy_str method_name, toy_expr_list *args)
{
    toy_expr *expr;
    expr = mymalloc(toy_expr);
    expr->type = EXPR_METHOD_CALL;
    expr->method_call.id.name = lhs;
    expr->method_call.method_name = method_name;
    expr->method_call.args = args;
    return expr;
}

toy_bool toy_expr_equal(const toy_expr *expr1, const toy_expr *expr2)
{
    return 0 == memcmp(expr1, expr2, sizeof(*expr1));
}

#ifndef NDEBUG

void expr_type_assert_valid(toy_expr_type type)
{
    assert(type >= 0);
    assert(type <= EXPR_MAX);
}

void expr_assert_valid(const toy_expr *expr)
{
    expr_type_assert_valid(expr->type);
    /* TODO */
}

#endif /* NDEBUG */

static void dump_binary_op(const toy_expr *arg1, const toy_expr *arg2, const char *op)
{
    log_putc(LOG_DEBUG, '(');
    expr_dump(arg1);
    log_debug(op);
    expr_dump(arg2);
    log_putc(LOG_DEBUG, ')');
}

static void dump_assignment(const toy_str lhs, const toy_expr *rhs)
{
    str_dump(lhs, TOY_FALSE);
    log_debug(" = ");
    expr_dump(rhs);
}

static void dump_collection_lookup(const toy_str lhs, const toy_expr *rhs)
{
    str_dump(lhs, TOY_FALSE);
    log_putc(LOG_DEBUG, '[');
    expr_dump(rhs);
    log_putc(LOG_DEBUG, ']');
}

static void dump_function_call(const toy_str func_name, const toy_expr_list *func_args)
{
    log_debug("%s(", func_name);
    expr_list_dump(func_args, TOY_FALSE);
    log_putc(LOG_DEBUG, ')');
}

static void dump_method_call(const toy_method_call *method_call)
{
    log_debug("%s.%s(", method_call->id.name, method_call->method_name);
    expr_list_dump(method_call->args, TOY_FALSE);
    log_putc(LOG_DEBUG, ')');
}

static void dump_identifier(const toy_str id)
{
    print_str(id);
}

void expr_dump(const toy_expr *expr) {
    if (expr) {
        switch (expr->type) {
        case EXPR_AND:
            dump_binary_op(expr->binary_op.arg1, expr->binary_op.arg2, " and ");
            break;
        case EXPR_ASSIGN:
            dump_assignment(expr->assignment.id.name, expr->assignment.rhs);
            break;
        case EXPR_COLLECTION_LOOKUP:
            dump_collection_lookup(expr->collection_lookup.id.name, expr->collection_lookup.rhs);
            break;
        case EXPR_COMMA:
            dump_binary_op(expr->binary_op.arg1, expr->binary_op.arg2, ", ");
            break;
        case EXPR_DIV:
            dump_binary_op(expr->binary_op.arg1, expr->binary_op.arg2, " / ");
            break;
        case EXPR_EQUAL:
            dump_binary_op(expr->binary_op.arg1, expr->binary_op.arg2, " == ");
            break;
        case EXPR_EXPONENT:
            dump_binary_op(expr->binary_op.arg1, expr->binary_op.arg2, " ** ");
            break;
        case EXPR_FIELD_REF:
            /* TODO */
            break;
        case EXPR_FUNC_CALL:
            dump_function_call(expr->func_call.id.name, expr->func_call.args);
            break;
        case EXPR_GT:
            dump_binary_op(expr->binary_op.arg1, expr->binary_op.arg2, " > ");
            break;
        case EXPR_GTE:
            dump_binary_op(expr->binary_op.arg1, expr->binary_op.arg2, " >= ");
            break;
        case EXPR_IDENTIFIER:
            log_debug("%s", expr->id.name);
            break;
        case EXPR_IN:
            dump_binary_op(expr->binary_op.arg1, expr->binary_op.arg2, " in ");
            break;
        case EXPR_LIST:
            expr_list_dump(expr->expr_list, TOY_TRUE);
            break;
        case EXPR_LITERAL:
            val_dump(expr->val, 1);
            break;
        case EXPR_LT:
            dump_binary_op(expr->binary_op.arg1, expr->binary_op.arg2, " < ");
            break;
        case EXPR_LTE:
            dump_binary_op(expr->binary_op.arg1, expr->binary_op.arg2, " <= ");
            break;
        case EXPR_MAP:
            map_expr_entry_list_dump(expr->obj_expr);
            break;
        case EXPR_METHOD_CALL:
            dump_method_call(&expr->method_call);
            break;
        case EXPR_MINUS:
            dump_binary_op(expr->binary_op.arg1, expr->binary_op.arg2, " - ");
            break;
        case EXPR_MODULUS:
            dump_binary_op(expr->binary_op.arg1, expr->binary_op.arg2, " % ");
            break;
        case EXPR_MUL:
            dump_binary_op(expr->binary_op.arg1, expr->binary_op.arg2, " * ");
            break;
        case EXPR_NEQUAL:
            dump_binary_op(expr->binary_op.arg1, expr->binary_op.arg2, " != ");
            break;
        case EXPR_NOT:
            log_puts(LOG_DEBUG, "not (");
            expr_dump(expr->unary_op.arg);
            log_puts(LOG_DEBUG, ")");
            break;
        case EXPR_OR:
            dump_binary_op(expr->binary_op.arg1, expr->binary_op.arg2, " or ");
            break;
        case EXPR_PLUS:
            dump_binary_op(expr->binary_op.arg1, expr->binary_op.arg2, " + ");
            break;
        case EXPR_POSTFIX_DECREMENT:
            dump_identifier(expr->postfix_decrement.id.name);
            log_puts(LOG_DEBUG, "--");
            break;
        case EXPR_POSTFIX_INCREMENT:
            dump_identifier(expr->postfix_increment.id.name);
            log_puts(LOG_DEBUG, "++");
            break;
        case EXPR_PREFIX_DECREMENT:
            log_puts(LOG_DEBUG, "--");
            dump_identifier(expr->prefix_decrement.id.name);
            break;
        case EXPR_PREFIX_INCREMENT:
            log_puts(LOG_DEBUG, "++");
            dump_identifier(expr->prefix_increment.id.name);
            break;
        case EXPR_TERNARY:
            expr_dump(expr->ternary.condition);
            log_puts(LOG_DEBUG, " ? ");
            expr_dump(expr->ternary.if_true);
            log_puts(LOG_DEBUG, " : ");
            expr_dump(expr->ternary.if_false);
            break;
        case EXPR_UNEG:
            log_puts(LOG_DEBUG, "-");
            expr_dump(expr->unary_op.arg);
            break;
        default:
            invalid_expr_type(expr->type);
            break;
        }
    } else {
        /* TODO: Use null_expr */
        log_debug("null");
    }
}
