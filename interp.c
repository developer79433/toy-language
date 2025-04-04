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
#include "constants.h"
#include "functions.h"
#include "errors.h"

toy_bool convert_to_bool(const toy_val *val)
{
    switch (val->type) {
    case VAL_BOOL:
        return val->bool;
    case VAL_NULL:
        return 0;
    case VAL_NUM:
        return val->num != 0;
    case VAL_LIST:
        return list_len(val->list) != 0;
    case VAL_MAP:
        return map_len(val->map) != 0;
    case VAL_STR:
        return strlen(val->str) != 0;
    default:
        invalid_expr_type(val->type);
        break;
    }
    return 0;
}

static toy_bool convert_to_num(const toy_val *val)
{
    switch (val->type) {
    case VAL_BOOL:
        return val->bool ? TOY_TRUE : TOY_FALSE;
    case VAL_NULL:
        return 0;
    case VAL_NUM:
        return val->num;
    case VAL_FUNC:
    case VAL_LIST:
    case VAL_MAP:
    case VAL_STR:
        invalid_argument_type(VAL_BOOL, val);
        break;
    default:
        invalid_value_type(val->type);
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

int lookup_identifier(toy_interp *interp, toy_expr *result, const toy_str name)
{
    const toy_expr *predef_const = lookup_predefined_constant(name);
    if (predef_const) {
        *result = *((toy_expr *) predef_const);
        return 1;
    }
    /* TODO: Pre-wrap the builtin functions at compile time, so we don't have to do it here at runtime */
    const predefined_function *predef_func = lookup_predefined_function(name);
    if (predef_func) {
        result->type = EXPR_LITERAL;
        result->val.type = VAL_FUNC;
        result->val.func.def.name = predef_func->name;
        result->val.func.def.code.stmts = NULL;
        result->val.func.def.param_names = NULL;
        for (int i = 0; i < predef_func->num_params; i++) {
            toy_str_list *entry = alloc_str_list(predef_func->param_names[i]);
            entry->next = result->val.func.def.param_names;
            result->val.func.def.param_names = entry;
        }
        return 1;
    }
    toy_expr *existing_value = map_get(interp->symbols, name);
    if (existing_value) {
        *result = *existing_value;
        return 1;
    }
    return 0;
}

static void set_variable(toy_interp *interp, const toy_str name, const toy_expr *value)
{
    if (is_predefined(name)) {
        readonly_identifier(name);
    }
    toy_expr old_value;
    int already_exists = lookup_identifier(interp, &old_value, name);
    if (!already_exists) {
        undeclared_identifier(name);
    }
    toy_expr value_result;
    if (value) {
        eval_expr(interp, &value_result, value);
    } else {
        value_result = null_expr;
    }
    map_set(interp->symbols, name, &value_result);
}

static void add_variable(toy_interp *interp, const toy_str name, const toy_expr *value)
{
    if (is_predefined(name)) {
        readonly_identifier(name);
    }
    toy_expr old_value;
    int already_exists = lookup_identifier(interp, &old_value, name);
    if (already_exists) {
        duplicate_identifier(name);
    }
    toy_expr value_result;
    if (value) {
        eval_expr(interp, &value_result, value);
    } else {
        value_result = null_expr;
    }
    map_set(interp->symbols, name, &value_result);
}

static void add_function(toy_interp *interp, const toy_str name, const toy_func_def *def)
{
    if (is_predefined(name)) {
        readonly_identifier(name);
    }
    toy_expr old_value;
    int already_exists = lookup_identifier(interp, &old_value, name);
    if (already_exists) {
        duplicate_identifier(name);
    }
    toy_expr *func_expr = alloc_literal(VAL_FUNC);
    memcpy(&func_expr->val.func.def, def, sizeof(*def));
    map_set(interp->symbols, name, func_expr);
}

static void op_assign(toy_interp *interp, toy_expr *result, toy_str name, toy_expr *arg)
{
    if (is_predefined(name)) {
        readonly_identifier(name);
    }
    toy_expr arg_result;
    eval_expr(interp, &arg_result, arg);
    assert(EXPR_LITERAL == arg_result.type);
    toy_expr old_value;
    int already_exists = lookup_identifier(interp, &old_value, name);
    if (already_exists) {
        map_set(interp->symbols, name, &arg_result);
        *result = arg_result;
    } else {
        undeclared_identifier(name);
    }
}

void run_toy_function(toy_interp *interp, toy_expr *result, toy_block *block, toy_str_list *arg_name, toy_list *args)
{
    push_context(interp, block);
    for (; arg_name && args; arg_name = arg_name->next, args = args->next) {
        /* TODO: This should add new variables, because this should be a fresh scope */
        set_variable(interp, arg_name->str, args->expr);
    }
    run_block(interp, block);
    pop_context(interp);
    /* TODO: Propagate return value from function */
    result->type = EXPR_LITERAL;
    result->val.type = VAL_NULL;
}

static size_t func_def_num_params(const toy_func_def *def)
{
    if (def->param_names) {
        return str_list_len(def->param_names);
    }
    return 0;
}

static size_t func_call_num_args(const toy_list *args)
{
    if (args) {
        return list_len(args);
    }
    return 0;
}

void call_func(toy_interp *interp, toy_expr *result, toy_str func_name, toy_list *args)
{
    size_t args_len = func_call_num_args(args);
    const predefined_function *predef = lookup_predefined_function(func_name);
    if (predef) {
        if (predef->num_params == INFINITE_PARAMS) {
            predef->func(interp, result, args);
        } else if (args_len < predef->num_params) {
            too_few_arguments(predef->num_params, args);
        } else if (args_len > predef->num_params) {
            too_many_arguments(predef->num_params, args);
        } else {
            predef->func(interp, result, args);
            /* TODO: Propagate return value from function */
            result->type = EXPR_LITERAL;
            result->val.type = VAL_NULL;
        }
    } else {
        toy_expr expr;
        int already_exists = lookup_identifier(interp, &expr, func_name);
        if (already_exists) {
            assert(EXPR_LITERAL == expr.type);
            if (expr.val.type == VAL_FUNC) {
                toy_func_def *def = &expr.val.func.def;
                size_t num_params = func_def_num_params(def);
                if (args_len < num_params) {
                    too_few_arguments(num_params, args);
                } else if (args_len > num_params) {
                    too_many_arguments(num_params, args);
                } else {
                    run_toy_function(interp, result, &def->code, def->param_names, args);
                }
            } else {
                invalid_operand(EXPR_FUNC_CALL, &expr);
            }
        } else {
            undeclared_identifier(func_name);
        }
    }
}

static void collection_lookup(toy_interp *interp, toy_expr *result, toy_str identifier, toy_expr *index)
{
    toy_expr collection;
    int already_exists = lookup_identifier(interp, &collection, identifier);
    if (already_exists) {
        assert(EXPR_LITERAL == collection.type);
        if (collection.val.type == VAL_LIST) {
            toy_expr index_result;
            eval_expr(interp, &index_result, index);
            assert(EXPR_LITERAL == index_result.type);
            if (index_result.val.type == VAL_NUM) {
                *result = *list_index(collection.val.list, index_result.val.num);
            } else {
                invalid_operand(EXPR_COLLECTION_LOOKUP, &index_result);
            }
        } else if (collection.val.type == VAL_MAP) {
            toy_expr index_result;
            eval_expr(interp, &index_result, index);
            assert(EXPR_LITERAL == index_result.type);
            if (index_result.val.type == VAL_STR) {
                *result = *map_get(collection.val.map, index_result.val.str);
            } else {
                invalid_operand(EXPR_COLLECTION_LOOKUP, &index_result);
            }
        } else if (collection.val.type == VAL_STR) {
            toy_expr index_result;
            eval_expr(interp, &index_result, index);
            assert(EXPR_LITERAL == index_result.type);
            if (index_result.val.type == VAL_NUM) {
                if (index_result.val.num >= 0 && index_result.val.num < strlen(collection.val.str)) {
                    result->type = EXPR_LITERAL;
                    result->val.type = VAL_STR;
                    /* TODO: Hide string memory management */
                    result->val.str = malloc(2);
                    result->val.str[0] = collection.val.str[(int) index_result.val.num];
                    result->val.str[1] = '\0';
                } else {
                    invalid_string_index(collection.val.str, index_result.val.num);
                }
            } else {
                invalid_operand(EXPR_COLLECTION_LOOKUP, &index_result);
            }
        } else {
            invalid_operand(EXPR_COLLECTION_LOOKUP, &collection);
        }
    } else {
        undeclared_identifier(identifier);
    }
}

/* FIXME: This actually does pre-increment */
static void op_postfix_decrement(toy_interp *interp, toy_expr *result, toy_str id)
{
    toy_expr cur_value;
    int found = lookup_identifier(interp, &cur_value, id);
    if (found) {
        assert(EXPR_LITERAL == cur_value.type);
        if (cur_value.val.type == VAL_NUM) {
            toy_expr new_value = { .type = EXPR_LITERAL, .val.type = VAL_NUM, .val.num = cur_value.val.num - 1 };
            set_variable(interp, id, &new_value);
            *result = cur_value;
        } else {
            invalid_operand(EXPR_POSTFIX_DECREMENT, &cur_value);
        }
    } else {
        undeclared_identifier(id);
    }
}

static void op_postfix_increment(toy_interp *interp, toy_expr *result, toy_str id)
{
    toy_expr cur_value;
    int found = lookup_identifier(interp, &cur_value, id);
    if (found) {
        assert(EXPR_LITERAL == cur_value.type);
        if (cur_value.val.type == VAL_NUM) {
            toy_expr new_value = { .type = EXPR_LITERAL, .val.type = VAL_NUM, .val.num = cur_value.val.num + 1 };
            set_variable(interp, id, &new_value);
            *result = cur_value;
        } else {
            invalid_operand(EXPR_POSTFIX_DECREMENT, &cur_value);
        }
    } else {
        undeclared_identifier(id);
    }
}

static void op_prefix_decrement(toy_interp *interp, toy_expr *result, toy_str id)
{
    toy_expr cur_value;
    int found = lookup_identifier(interp, &cur_value, id);
    if (found) {
        assert(EXPR_LITERAL == cur_value.type);
        if (cur_value.val.type == VAL_NUM) {
            toy_expr new_value = { .type = EXPR_LITERAL, .val.type = VAL_NUM, .val.num = cur_value.val.num - 1 };
            set_variable(interp, id, &new_value);
            *result = new_value;
        } else {
            invalid_operand(EXPR_POSTFIX_DECREMENT, &cur_value);
        }
    } else {
        undeclared_identifier(id);
    }
}

static void op_prefix_increment(toy_interp *interp, toy_expr *result, toy_str id)
{
    toy_expr cur_value;
    int found = lookup_identifier(interp, &cur_value, id);
    if (found) {
        assert(EXPR_LITERAL == cur_value.type);
        if (cur_value.val.type == VAL_NUM) {
            toy_expr new_value = { .type = EXPR_LITERAL, .val.type = VAL_NUM, .val.num = cur_value.val.num + 1 };
            set_variable(interp, id, &new_value);
            *result = new_value;
        } else {
            invalid_operand(EXPR_POSTFIX_DECREMENT, &cur_value);
        }
    } else {
        undeclared_identifier(id);
    }
}

/* TODO: Second param should be a toy_val * */
void eval_expr(toy_interp *interp, toy_expr *result, const toy_expr *expr)
{
    switch (expr->type) {
    case EXPR_AND:
        op_and(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        assert(EXPR_LITERAL == result->type);
        break;
    case EXPR_ASSIGN:
        op_assign(interp, result, expr->assignment.lhs, expr->assignment.rhs);
        assert(EXPR_LITERAL == result->type);
        break;
    case EXPR_COLLECTION_LOOKUP:
        collection_lookup(interp, result, expr->collection_lookup.lhs, expr->collection_lookup.rhs);
        assert(EXPR_LITERAL == result->type);
        break;
    case EXPR_COMMA:
        op_comma(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        assert(EXPR_LITERAL == result->type);
        break;
    case EXPR_DIV:
        op_div(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        assert(EXPR_LITERAL == result->type);
        break;
    case EXPR_EQUAL:
        op_equal(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        assert(EXPR_LITERAL == result->type);
        break;
    case EXPR_EXPONENT:
        op_exponent(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        assert(EXPR_LITERAL == result->type);
        break;
    case EXPR_FIELD_REF:
        op_field_ref(interp, result, expr->field_ref.lhs, expr->field_ref.rhs);
        assert(EXPR_LITERAL == result->type);
        break;
    case EXPR_FUNC_CALL:
        call_func(interp, result, expr->func_call.func_name, expr->func_call.args);
        assert(EXPR_LITERAL == result->type);
        break;
    case EXPR_GT:
        op_gt(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        assert(EXPR_LITERAL == result->type);
        break;
    case EXPR_GTE:
        op_gte(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        assert(EXPR_LITERAL == result->type);
        break;
    case EXPR_IDENTIFIER:
        int found = lookup_identifier(interp, result, expr->val.str);
        if (!found) {
            undeclared_identifier(expr->val.str);
        }
        assert(EXPR_LITERAL == result->type);
        break;
    case EXPR_IN:
        op_in(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        assert(EXPR_LITERAL == result->type);
        break;
    case EXPR_LITERAL:
        result->type = EXPR_LITERAL;
        result->val = expr->val;
        break;
    case EXPR_LT:
        op_lt(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        assert(EXPR_LITERAL == result->type);
        break;
    case EXPR_LTE:
        op_lte(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        assert(EXPR_LITERAL == result->type);
        break;
    case EXPR_METHOD_CALL:
        op_method_call(interp, result, expr->method_call.target, expr->method_call.func_name, expr->method_call.args);
        assert(EXPR_LITERAL == result->type);
        break;
    case EXPR_MINUS:
        op_minus(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        assert(EXPR_LITERAL == result->type);
        break;
    case EXPR_MODULUS:
        op_modulus(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        assert(EXPR_LITERAL == result->type);
        break;
    case EXPR_MUL:
        op_mul(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        assert(EXPR_LITERAL == result->type);
        break;
    case EXPR_NEQUAL:
        op_equal(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        result->val.bool = !result->val.bool;
        assert(EXPR_LITERAL == result->type);
        break;
    case EXPR_NOT:
        op_not(interp, result, expr->unary_op.arg);
        assert(EXPR_LITERAL == result->type);
        break;
    case EXPR_OR:
        op_or(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        assert(EXPR_LITERAL == result->type);
        break;
    case EXPR_PLUS:
        op_plus(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        assert(EXPR_LITERAL == result->type);
        break;
    case EXPR_POSTFIX_DECREMENT:
        op_postfix_decrement(interp, result, expr->postfix_decrement.id);
        assert(EXPR_LITERAL == result->type);
        break;
    case EXPR_POSTFIX_INCREMENT:
        op_postfix_increment(interp, result, expr->postfix_increment.id);
        assert(EXPR_LITERAL == result->type);
        break;
    case EXPR_PREFIX_DECREMENT:
        op_prefix_decrement(interp, result, expr->prefix_decrement.id);
        assert(EXPR_LITERAL == result->type);
        break;
    case EXPR_PREFIX_INCREMENT:
        op_prefix_increment(interp, result, expr->prefix_increment.id);
        assert(EXPR_LITERAL == result->type);
        break;
    case EXPR_TERNARY:
        op_ternary(interp, result, expr->ternary.condition, expr->ternary.if_true, expr->ternary.if_false);
        assert(EXPR_LITERAL == result->type);
        break;
    case EXPR_UNEG:
        op_uneg(interp, result, expr->unary_op.arg);
        assert(EXPR_LITERAL == result->type);
        break;
    default:
        invalid_expr_type(expr->type);
        break;
    }
    assert(result->type == EXPR_LITERAL);
}

static void for_stmt(toy_interp *interp, const toy_for_stmt *for_stmt) {
    if (for_stmt->at_start) {
        /* TODO: Can be run_block, because ->next should be NULL */
        single_step(interp, for_stmt->at_start);
    }
    for (;;) {
        if (for_stmt->condition) {
            toy_expr cond_result;
            eval_expr(interp, &cond_result, for_stmt->condition);
            assert(EXPR_LITERAL == cond_result.type);
            if (cond_result.val.type != VAL_BOOL) {
                invalid_argument_type(VAL_BOOL, &cond_result.val);
            }
            assert(VAL_BOOL == cond_result.val.type);
            if (!cond_result.val.bool) {
                break;
            }
        }
        run_block(interp, &for_stmt->body);
        if (for_stmt->at_end) {
            /* TODO: Can be run_block, because ->next should be NULL */
            single_step(interp, for_stmt->at_end);
        }
    }
}

static void if_stmt(toy_interp *interp, const toy_if_stmt *if_stmt)
{
    unsigned int found_one = 0;
    for (toy_if_arm *arm = if_stmt->arms; arm; arm = arm->next) {
        toy_expr cond_result;
        eval_expr(interp, &cond_result, arm->condition);
        assert(EXPR_LITERAL == cond_result.type);
        if (VAL_BOOL != cond_result.val.type) {
            invalid_argument_type(VAL_BOOL, &cond_result.val);
        }
        assert(VAL_BOOL == cond_result.val.type);
        if (cond_result.val.bool) {
            run_block(interp, &arm->code);
            found_one = 1;
            break;
        }
    }
    if (!found_one && if_stmt->elsepart.stmts) {
        run_block(interp, &if_stmt->elsepart);
    }
}

static void while_stmt(toy_interp *interp, const toy_while_stmt *while_stmt)
{
    for(;;) {
        toy_expr cond_result;
        eval_expr(interp, &cond_result, while_stmt->condition);
        assert(EXPR_LITERAL == cond_result.type);
        if (VAL_BOOL != cond_result.val.type) {
            invalid_operand(VAL_BOOL, &cond_result);
        }
        assert(VAL_BOOL == cond_result.val.type);
        if (cond_result.val.bool) {
            break;
        }
        run_block(interp, &while_stmt->body);
    }
}

/**
 * TODO: Should return an enum representing the decision to continue executing or not,
 * eg when debugging, or executing return statements
 */
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
        for_stmt(interp, &stmt->for_stmt);
        break;
    case STMT_FUNC_DECL:
        add_function(interp, stmt->func_decl_stmt.def.name, &stmt->func_decl_stmt.def);
        break;
    case STMT_IF:
        if_stmt(interp, &stmt->if_stmt);
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
        while_stmt(interp, &stmt->while_stmt);
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
