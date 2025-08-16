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
    "map",
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
    toy_expr *expr;
    expr = mymalloc(toy_expr);
    expr->type = EXPR_LITERAL;
    expr->val.type = val_type;
    return expr;
}

toy_expr *alloc_expr_func_decl(toy_str_list *formalparams, toy_block *block)
{
    toy_expr *expr;
    expr = (toy_expr *) malloc(sizeof(toy_expr) + sizeof(toy_function));
    expr->type = EXPR_LITERAL;
    expr->val.type = VAL_FUNC;
    expr->val.func = (toy_function *) (expr + 1);
    expr->val.func->type = FUNC_USER_DECLARED;
    expr->val.func->name = ""; /* TODO: generated unique name */
    expr->val.func->parent = NULL;
    expr->val.func->code.stmts = block->stmts;
    expr->val.func->param_names = formalparams;
    return expr;
}

toy_expr *alloc_expr_func_call(toy_str id, toy_expr_list *args)
{
    toy_expr *expr;
    expr = mymalloc(toy_expr);
    expr->type = EXPR_FUNC_CALL;
    expr->func_call.id = id;
    expr->func_call.args = args;
    return expr;
}

toy_expr *alloc_expr_method_call(toy_str lhs, toy_str method_name, toy_expr_list *args)
{
    toy_expr *expr;
    expr = mymalloc(toy_expr);
    expr->type = EXPR_METHOD_CALL;
    expr->method_call.lhs = lhs;
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

static void dump_binary_op(FILE *f, const toy_expr *arg1, const toy_expr *arg2, const char *op)
{
    fputc('(', f);
    expr_dump(f, arg1);
    fputs(op, f);
    expr_dump(f, arg2);
    fputc(')', f);
}

static void dump_assignment(FILE *f, const toy_str lhs, const toy_expr *rhs)
{
    dump_str(f, lhs);
    fputs(" = ", f);
    expr_dump(f, rhs);
}

static void dump_collection_lookup(FILE *f, const toy_str lhs, const toy_expr *rhs)
{
    dump_str(f, lhs);
    fputc('[', f);
    expr_dump(f, rhs);
    fputc(']', f);
}

typedef struct expr_dump_cb_args_struct {
    FILE *f;
    unsigned int output_something;
} expr_dump_cb_args;

static item_callback_result expr_dump_callback(void *cookie, size_t index, const toy_expr_list *item)
{
    expr_dump_cb_args *args = (expr_dump_cb_args *) cookie;
    const toy_expr *expr = expr_list_payload_const(item);
    if (args->output_something) {
        fputs(", ", args->f);
    }
    expr_dump(args->f, expr);
    args->output_something = 1;
    return CONTINUE_ENUMERATION;
}

static void dump_function_call(FILE *f, const toy_str func_name, const toy_expr_list *func_args)
{
    fprintf(f, "%s(", func_name);
    expr_dump_cb_args cb_args = { .f = f, .output_something = 0 };
    enumeration_result res = expr_list_foreach_const(func_args, expr_dump_callback, &cb_args);
    assert(ENUMERATION_COMPLETE == res);
    fputc(')', f);
}

static void dump_method_call(FILE *f, const toy_method_call *method_call)
{
    fprintf(f, "%s.%s(", method_call->lhs, method_call->method_name);
    unsigned int output_something = 0;
    for (toy_expr_list *arg = method_call->args; arg; arg = arg->next) {
        if (output_something) {
            fputs(", ", f);
        }
        expr_dump(f, arg->expr);
        output_something = 1;
    }
    fputc(')', f);
}

void expr_dump(FILE *f, const toy_expr *expr) {
    if (expr) {
        switch (expr->type) {
        case EXPR_AND:
            dump_binary_op(f, expr->binary_op.arg1, expr->binary_op.arg2, " and ");
            break;
        case EXPR_ASSIGN:
            dump_assignment(f, expr->assignment.lhs, expr->assignment.rhs);
            break;
        case EXPR_COLLECTION_LOOKUP:
            dump_collection_lookup(f, expr->collection_lookup.lhs, expr->collection_lookup.rhs);
            break;
        case EXPR_COMMA:
            dump_binary_op(f, expr->binary_op.arg1, expr->binary_op.arg2, ", ");
            break;
        case EXPR_DIV:
            dump_binary_op(f, expr->binary_op.arg1, expr->binary_op.arg2, " / ");
            break;
        case EXPR_EQUAL:
            dump_binary_op(f, expr->binary_op.arg1, expr->binary_op.arg2, " == ");
            break;
        case EXPR_EXPONENT:
            dump_binary_op(f, expr->binary_op.arg1, expr->binary_op.arg2, " ** ");
            break;
        case EXPR_FIELD_REF:
            /* TODO */
            break;
        case EXPR_FUNC_CALL:
            dump_function_call(f, expr->func_call.id, expr->func_call.args);
            break;
        case EXPR_GT:
            dump_binary_op(f, expr->binary_op.arg1, expr->binary_op.arg2, " > ");
            break;
        case EXPR_GTE:
            dump_binary_op(f, expr->binary_op.arg1, expr->binary_op.arg2, " >= ");
            break;
        case EXPR_IDENTIFIER:
            fprintf(f, "%s", expr->id.id);
            break;
        case EXPR_IN:
            dump_binary_op(f, expr->binary_op.arg1, expr->binary_op.arg2, " in ");
            break;
        case EXPR_LIST:
            expr_list_dump(f, expr->list);
            break;
        case EXPR_LITERAL:
            val_dump(f, &expr->val);
            break;
        case EXPR_LT:
            dump_binary_op(f, expr->binary_op.arg1, expr->binary_op.arg2, " < ");
            break;
        case EXPR_LTE:
            dump_binary_op(f, expr->binary_op.arg1, expr->binary_op.arg2, " <= ");
            break;
        case EXPR_MAP:
            map_entry_list_dump(f, expr->map);
            break;
        case EXPR_METHOD_CALL:
            dump_method_call(f, &expr->method_call);
            break;
        case EXPR_MINUS:
            dump_binary_op(f, expr->binary_op.arg1, expr->binary_op.arg2, " - ");
            break;
        case EXPR_MODULUS:
            dump_binary_op(f, expr->binary_op.arg1, expr->binary_op.arg2, " % ");
            break;
        case EXPR_MUL:
            dump_binary_op(f, expr->binary_op.arg1, expr->binary_op.arg2, " * ");
            break;
        case EXPR_NEQUAL:
            dump_binary_op(f, expr->binary_op.arg1, expr->binary_op.arg2, " != ");
            break;
        case EXPR_NOT:
            fputs("not (", f);
            expr_dump(f, expr->unary_op.arg);
            fputs(")", f);
            break;
        case EXPR_OR:
            dump_binary_op(f, expr->binary_op.arg1, expr->binary_op.arg2, " or ");
            break;
        case EXPR_PLUS:
            dump_binary_op(f, expr->binary_op.arg1, expr->binary_op.arg2, " + ");
            break;
        case EXPR_POSTFIX_DECREMENT:
            dump_str(f, expr->postfix_decrement.id);
            fputs("--", f);
            break;
        case EXPR_POSTFIX_INCREMENT:
            dump_str(f, expr->postfix_increment.id);
            fputs("++", f);
            break;
        case EXPR_PREFIX_DECREMENT:
            fputs("--", f);
            dump_str(f, expr->prefix_decrement.id);
            break;
        case EXPR_PREFIX_INCREMENT:
            fputs("++", f);
            dump_str(f, expr->prefix_increment.id);
            break;
        case EXPR_TERNARY:
            expr_dump(f, expr->ternary.condition);
            fputs(" ? ", f);
            expr_dump(f, expr->ternary.if_true);
            fputs(" : ", f);
            expr_dump(f, expr->ternary.if_false);
            break;
        case EXPR_UNEG:
            fputs("-", f);
            expr_dump(f, expr->unary_op.arg);
            break;
        default:
            invalid_expr_type(expr->type);
            break;
        }
    } else {
        /* TODO: Use null_expr */
        fputs("null", f);
    }
}
