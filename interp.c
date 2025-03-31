#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include "interp.h"
#include "list.h"
#include "map.h"
#include "util.h"
#include "dump.h"
#include "operations.h"

toy_bool convert_to_bool(const toy_expr *expr)
{
    switch (expr->type) {
    case EXPR_ASSIGN:
        /* TODO */
        break;
    case EXPR_BOOL:
        return expr->bool;
    case EXPR_NUM:
        return expr->num != 0;
    case EXPR_LIST:
        return list_len(expr->list) != 0;
    case EXPR_MAP:
        return map_len(expr->map) != 0;
    case EXPR_STR:
        return strlen(expr->str) != 0;
    case EXPR_IDENTIFIER:
    case EXPR_AND:
    case EXPR_DIV:
    case EXPR_EQUAL:
    case EXPR_FUNC_CALL:
    case EXPR_FUNC_DECL:
    case EXPR_GT:
    case EXPR_GTE:
    case EXPR_LT:
    case EXPR_LTE:
    case EXPR_IN:
    case EXPR_MINUS:
    case EXPR_MUL:
    case EXPR_NEQUAL:
    case EXPR_NOT:
    case EXPR_OR:
    case EXPR_PLUS:
    case EXPR_UNEG:
        invalid_cast(EXPR_BOOL, expr);
        return 0;
    default:
        invalid_expr_type(expr->type);
        break;
    }
    return 0;
}

toy_bool convert_to_num(const toy_expr *expr)
{
    switch (expr->type) {
    case EXPR_ASSIGN:
        /* TODO */
        break;
    case EXPR_BOOL:
        return expr->bool ? 1 : 0;
    case EXPR_NUM:
        return expr->num;
    case EXPR_LIST:
    case EXPR_MAP:
    case EXPR_STR:
    case EXPR_IDENTIFIER:
    case EXPR_AND:
    case EXPR_DIV:
    case EXPR_EQUAL:
    case EXPR_FUNC_CALL:
    case EXPR_FUNC_DECL:
    case EXPR_GT:
    case EXPR_GTE:
    case EXPR_LT:
    case EXPR_LTE:
    case EXPR_IN:
    case EXPR_MINUS:
    case EXPR_MUL:
    case EXPR_NEQUAL:
    case EXPR_NOT:
    case EXPR_OR:
    case EXPR_PLUS:
    case EXPR_UNEG:
        invalid_cast(EXPR_BOOL, expr);
        return 0;
    default:
        invalid_expr_type(expr->type);
        break;
    }
    return 0;
}

static void push_context(toy_interp *interp, const toy_block *block)
{
    /* TODO: This needs to be a stack */
    interp->cur_block = block;
}

static void pop_context(toy_interp *interp)
{
    /* TODO */
}

static toy_expr null_expr = { .type = EXPR_NULL };
static toy_expr true_expr = { .type = EXPR_BOOL, .num = 1 };
static toy_expr false_expr = { .type = EXPR_BOOL, .num = 0 };
typedef struct predefined_constant_struct {
    toy_str name;
    toy_expr *value;
} predefined_constant;

static predefined_constant predefined_constants[] = {
    { "null", &null_expr },
    { "true", &true_expr },
    { "false", &false_expr }
};

static toy_expr *lookup_predefined_constant(toy_str name)
{
    for (
        const predefined_constant *constant = &predefined_constants[0];
        constant < &predefined_constants[ELEMENTSOF(predefined_constants)];
        constant++)
    {
        if (0 == strcasecmp(constant->name, name)) {
            return constant->value;
        }
    }
    return NULL;
}

typedef void (*predefined_func_addr)(toy_interp *interp, toy_expr *result, toy_list *args);

typedef struct predefined_function_struct {
    toy_str name;
    int num_params;
    predefined_func_addr func;
    const toy_char **param_names;
} predefined_function;

#define INFINITE_PARAMS -1

static void predefined_list_len(toy_interp *interp, toy_expr *result, toy_list *args)
{
    if (args->next) {
        too_many_arguments(1, args);
    }
    toy_expr *arg1 = args->expr;
    if (arg1->type != EXPR_LIST) {
        invalid_operand(EXPR_LIST, arg1);
    }
    result->type = EXPR_NUM;
    result->num = list_len(arg1->list);
}

static const char *predefined_list_len_arg_names[] = {
    "list"
};

static void predefined_print(toy_interp *interp, toy_expr *result, toy_list *args)
{
    for (toy_list *arg = args; arg; arg = arg->next) {
        toy_expr arg_result;
        eval_expr(interp, &arg_result, arg->expr);
        if (arg_result.type == EXPR_STR) {
            print_str(stderr, arg_result.str);
        } else {
            dump_expr(stderr, &arg_result);
        }
        fputc('\n', stderr);
    }
    *result = null_expr;
}

static predefined_function predefined_functions[] = {
    { .name = "len",   .num_params = 1,               .func = predefined_list_len, .param_names = predefined_list_len_arg_names },
    { .name = "print", .num_params = INFINITE_PARAMS, .func = predefined_print,    .param_names = NULL }
};

static predefined_func_addr lookup_predefined_function(toy_str name)
{
    for (
        const predefined_function *function = &predefined_functions[0];
        function < &predefined_functions[ELEMENTSOF(predefined_functions)];
        function++)
    {
        if (0 == strcasecmp(function->name, name)) {
            return function->func;
        }
    }
    return 0;
}

static int is_predefined(toy_str name)
{
    if (lookup_predefined_constant(name)) {
        return 1;
    }
    if (lookup_predefined_function(name)) {
        return 1;
    }
    return 0;
}

static toy_expr *lookup_identifier(toy_interp *interp, const toy_str name)
{
    for (
        const predefined_constant *constant = &predefined_constants[0];
        constant < &predefined_constants[ELEMENTSOF(predefined_constants)];
        constant++)
    {
        if (0 == strcasecmp(constant->name, name)) {
            return constant->value;
        }
    }
    predefined_func_addr func = lookup_predefined_function(name);
    if (func) {
        return NULL; // TODO: return predefined function
    }
    return map_get(interp->symbols, name);
}

static void set_variable(toy_interp *interp, const toy_str name, const toy_expr *value)
{
    if (is_predefined(name)) {
        readonly_identifier(name);
    }
    toy_expr *old_value = lookup_identifier(interp, name);
    if (!old_value) {
        undeclared_identifier(name);
    }
    toy_expr *new_value;
    if (value) {
        new_value = (toy_expr *) value;
    } else {
        new_value = &null_expr;
    }
    map_set(interp->symbols, name, new_value);
}

static void add_variable(toy_interp *interp, const toy_str name, const toy_expr *value)
{
    if (is_predefined(name)) {
        readonly_identifier(name);
    }
    toy_expr *existing_value = lookup_identifier(interp, name);
    if (existing_value) {
        duplicate_identifier(name);
    }
    toy_expr *new_value;
    if (value) {
        new_value = (toy_expr *) value;
    } else {
        new_value = &null_expr;
    }
    map_set(interp->symbols, name, new_value);
}

static void add_function(toy_interp *interp, const toy_str name, const toy_func_def *def)
{
    if (is_predefined(name)) {
        readonly_identifier(name);
    }
    toy_expr *existing_value = lookup_identifier(interp, name);
    if (existing_value) {
        duplicate_identifier(name);
    }
    toy_expr *func_expr = alloc_expr(EXPR_FUNC_DECL);
    memcpy(&func_expr->func_decl.def, def, sizeof(*def));
    map_set(interp->symbols, name, func_expr);
}

static void op_assign(toy_interp *interp, toy_expr *result, toy_str name, toy_expr *new_value)
{
    if (is_predefined(name)) {
        readonly_identifier(name);
    }
    toy_expr *old_value = lookup_identifier(interp, name);
    if (old_value) {
        map_set(interp->symbols, name, new_value);
        *result = *new_value;
    } else {
        undeclared_identifier(name);
    }
}

static void run_toy_function(toy_interp *interp, toy_expr *result, toy_block *block, toy_str_list *arg_name, toy_list *arg)
{
    push_context(interp, block);
    /* TODO: set arguments into block environment */
    for (; arg_name && arg; arg_name = arg_name->next, arg = arg->next) {
        // TODO: set_variable(interp, arg_name, arg);
    }
    run_block(interp, block);
    pop_context(interp);
}

static void call_func(toy_interp *interp, toy_expr *result, toy_str func_name, toy_list *args)
{
    predefined_func_addr predef = lookup_predefined_function(func_name);
    if (predef) {
        (*predef)(interp, result, args);
    } else {
        toy_expr *expr = lookup_identifier(interp, func_name);
        if (expr->type == EXPR_FUNC_DECL) {
            run_toy_function(interp, result, &expr->func_decl.def.code, expr->func_decl.def.param_names, args);
        } else {
            invalid_operand(EXPR_FUNC_CALL, expr);
        }
    }
}

static void collection_lookup(toy_interp *interp, toy_expr *result, toy_str identifier, toy_expr *index)
{
    toy_expr *collection;
    collection = lookup_identifier(interp, identifier);
    if (!collection) {
        undeclared_identifier(identifier);
    }
    if (collection->type == EXPR_LIST) {
        toy_expr index_result;
        eval_expr(interp, &index_result, index);
        if (index_result.type == EXPR_NUM) {
            *result = *list_index(collection->list, index_result.num);
        } else {
            invalid_operand(EXPR_COLLECTION_LOOKUP, &index_result);
        }
    } else if (collection->type == EXPR_MAP) {
        toy_expr index_result;
        eval_expr(interp, &index_result, index);
        if (index_result.type == EXPR_STR) {
            *result = *map_get(collection->map, index_result.str);
        } else {
            invalid_operand(EXPR_COLLECTION_LOOKUP, &index_result);
        }
    } else {
        invalid_operand(EXPR_COLLECTION_LOOKUP, collection);
    }
}

void eval_expr(toy_interp *interp, toy_expr *result, const toy_expr *expr)
{
    switch (expr->type) {
    case EXPR_AND:
        op_and(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_ASSIGN:
        op_assign(interp, result, expr->assignment.lhs, expr->assignment.rhs);
        break;
    case EXPR_BOOL:
        *result = *expr;
        break;
    case EXPR_COLLECTION_LOOKUP:
        collection_lookup(interp, result, expr->collection_lookup.lhs, expr->collection_lookup.rhs);
        break;
    case EXPR_COMMA:
        op_comma(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_DIV:
        op_div(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_EQUAL:
        op_equal(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_EXPONENT:
        op_exponent(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_FUNC_CALL:
        call_func(interp, result, expr->func_call.func_name, expr->func_call.args);
        break;
    case EXPR_FUNC_DECL:
        *result = *expr;
        break;
    case EXPR_GT:
        op_gt(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_GTE:
        op_gte(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_IDENTIFIER:
        toy_expr *value = lookup_identifier(interp, expr->str);
        if (value) {
            *result = *value;
        } else {
            undeclared_identifier(expr->str);
        }
        break;
    case EXPR_IN:
        op_in(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_LIST:
        *result = *expr;
        break;
    case EXPR_LT:
        op_lt(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_LTE:
        op_lte(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_MAP:
        *result = *expr;
        break;
    case EXPR_MINUS:
        op_minus(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_MODULUS:
        op_modulus(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_MUL:
        op_mul(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_NEQUAL:
        op_equal(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        result->bool = !result->bool;
        break;
    case EXPR_NOT:
        op_not(interp, result, expr->unary_op.arg);
        break;
    case EXPR_NULL:
        *result = null_expr;
        break;
    case EXPR_NUM:
        *result = *expr;
        break;
    case EXPR_OR:
        op_or(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_PLUS:
        op_plus(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_POSTFIX_DECREMENT:
        /* TODO */
        break;
    case EXPR_POSTFIX_INCREMENT:
        /* TODO */
        break;
    case EXPR_PREFIX_DECREMENT:
        /* TODO */
        break;
    case EXPR_PREFIX_INCREMENT:
        /* TODO */
        break;
    case EXPR_STR:
        *result = *expr;
        break;
    case EXPR_TERNARY:
        op_ternary(interp, result, expr->ternary.condition, expr->ternary.if_true, expr->ternary.if_false);
        break;
    case EXPR_UNEG:
        op_uneg(interp, result, expr->unary_op.arg);
        break;
    default:
        invalid_expr_type(expr->type);
        break;
    }
}

void single_step(toy_interp *interp, const toy_stmt *stmt)
{
    switch (stmt->type) {
    case STMT_BLOCK:
        run_block(interp, &stmt->block_stmt.block);
        break;
    case STMT_BREAK:
        /* TODO */
        break;
    case STMT_CONTINUE:
        /* TODO */
        break;
    case STMT_EXPR:
        toy_expr result;
        eval_expr(interp, &result, stmt->expr_stmt.expr);
        break;
    case STMT_FOR:
        if (stmt->for_stmt.at_start) {
            single_step(interp, stmt->for_stmt.at_start);
        }
        for (;;) {
            if (stmt->for_stmt.condition) {
                toy_expr cond_result;
                eval_expr(interp, &cond_result, stmt->for_stmt.condition);
                if (EXPR_BOOL != cond_result.type) {
                    invalid_operand(EXPR_BOOL, &cond_result);
                }
                assert(EXPR_BOOL == cond_result.type);
                if (cond_result.bool) {
                    break;
                }
            }
            run_block(interp, &stmt->for_stmt.body);
            if (stmt->for_stmt.at_end) {
                single_step(interp, stmt->for_stmt.at_end);
            }
        }
        break;
    case STMT_FUNC_DECL:
        add_function(interp, stmt->func_decl_stmt.def.name, &stmt->func_decl_stmt.def);
        break;
    case STMT_IF:
        {
            unsigned int found_one = 0;
            for (toy_if_arm *arm = stmt->if_stmt.arms; arm; arm = arm->next) {
                toy_expr cond_result;
                eval_expr(interp, &cond_result, arm->condition);
                if (EXPR_BOOL != cond_result.type) {
                    invalid_operand(EXPR_BOOL, &cond_result);
                }
                assert(EXPR_BOOL == cond_result.type);
                if (cond_result.bool) {
                    run_block(interp, &arm->code);
                    found_one = 1;
                    break;
                }
            }
            if (!found_one && stmt->if_stmt.elsepart.stmts) {
                run_block(interp, &stmt->if_stmt.elsepart);
            }
        }
        break;
    case STMT_NULL:
        break;
    case STMT_RETURN:
        /* TODO */
        break;
    case STMT_VAR_DECL:
        add_variable(interp, stmt->var_decl_stmt->name, stmt->var_decl_stmt->value);
        break;
    case STMT_WHILE:
        for(;;) {
            toy_expr cond_result;
            eval_expr(interp, &cond_result, stmt->while_stmt.condition);
            if (EXPR_BOOL != cond_result.type) {
                invalid_operand(EXPR_BOOL, &cond_result);
            }
            assert(EXPR_BOOL == cond_result.type);
            if (cond_result.bool) {
                break;
            }
            run_block(interp, &stmt->while_stmt.body);
        }
        break;
    default:
        invalid_stmt_type(stmt->type);
        break;
    }
}

void step_out(toy_interp *interp)
{
    for (const toy_stmt *s = interp->cur_block->stmts; s; s = s->next) {
        single_step(interp, s);
    }
    // dump_map(stderr, interp->symbols);
}

void run_block(toy_interp *interp, const toy_block *block)
{
    push_context(interp, block);
    step_out(interp);
    pop_context(interp);
}

void init_interp(toy_interp *interp, const toy_stmt *program)
{
    interp->top_block.stmts = (toy_stmt *) program;
    interp->cur_block = &interp->top_block;
    interp->symbols = alloc_map();
}
