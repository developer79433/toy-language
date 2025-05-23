#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include "mymalloc.h"
#include "str.h"
#include "log.h"
#include "dump.h"
#include "interp.h"
#include "val.h"
#include "val-list.h"
#include "str-list.h"
#include "expr-list.h"
#include "generic-map.h"
#include "util.h"
#include "operations.h"
#include "constants.h"
#include "function.h"
#include "errors.h"
#include "stmt-list.h"
#include "map-val.h"
#include "var-decl-list.h"
#include "interp-for.h"
#include "interp-if.h"
#include "interp-return.h"
#include "interp-var-decl.h"
#include "interp-while.h"
#include "interp-frame.h"
#include "interp-symbol.h"
#include "predef-function.h"

#if 0
#define DEBUG_STACK 1
#endif

#if 1
#define DEBUG_VARIABLES 1
#endif

typedef struct toy_interp_struct {
    toy_function main_program;
    interp_frame_list *cur_frame;
    toy_val return_val;
} toy_interp;

static run_stmt_result block_stmt(toy_interp *interp, const toy_block *block);

interp_frame_list *interp_cur_frame(toy_interp *interp)
{
    return interp->cur_frame;
}

void interp_set_cur_frame(toy_interp *interp, interp_frame_list *frame)
{
    interp->cur_frame = frame;
}

typedef struct append_cb_args_struct {
    toy_interp *interp;
    toy_val *result;
} append_cb_args;

static item_callback_result append_val_list_callback(void *cookie, size_t index, toy_expr_list *list)
{
    append_cb_args *args = (append_cb_args *) cookie;
    toy_expr *expr = expr_list_payload(list);
    toy_val value;
    expr_eval(args->interp, &value, expr);
    assert(args->result->type == VAL_LIST);
    val_list_append(args->result->list, &value);
    return CONTINUE_ENUMERATION;
}

static void eval_expr_list(toy_interp *interp, toy_val *result, const toy_expr_list *expr_list)
{
    result->type = VAL_LIST;
    if (expr_list) {
        toy_val element;
        expr_eval(interp, &element, expr_list->expr);
        result->list = val_list_alloc(&element);
        append_cb_args append_args = { .interp = interp, .result = result };
        /* TODO: Remove this ugly -> next, which is here so we distinguish betwen initial list alloc and append cases */
        enumeration_result res = expr_list_foreach(expr_list->next, append_val_list_callback, &append_args);
        assert(ENUMERATION_COMPLETE == res);
    } else {
        result->list = NULL;
    }
}

static run_stmt_result run_predefined_func_val_list(toy_interp *interp, predefined_func_addr predef, const toy_val_list *args)
{
    return predef(interp, args);
}

static run_stmt_result run_predefined_func_expr_list(toy_interp *interp, predefined_func_addr predef, const toy_expr_list *arg_exprs)
{
    toy_val args;
    eval_expr_list(interp, &args, arg_exprs);
    assert(VAL_LIST == args.type);
    return run_predefined_func_val_list(interp, predef, args.list);
}

static run_stmt_result run_user_func_val_list(toy_interp *interp, toy_block *block, toy_str_list *param_names, const toy_val_list *args)
{
    val_list_assert_valid(args);
    /* TODO: Use str_list_foreach_const, but it doesn't support enumerating over two lists together, which is what we need here */
    for (toy_str_list *param_name = param_names; param_name && args; param_name = param_name->next, args = args->next) {
        val_assert_valid(&args->val);
        create_variable_value(interp, param_name->str, &args->val);
    }
    return run_current_block(interp);
}

static run_stmt_result run_user_func_expr_list(toy_interp *interp, toy_block *block, toy_str_list *param_names, const toy_expr_list *args)
{
    /* TODO: Use str_list_foreach_const, but it doesn't support enumerating over two lists together, which is what we need here */
    for (toy_str_list *param_name = param_names; param_name && args; param_name = param_name->next, args = args->next) {
        create_variable_expr(interp, param_name->str, args->expr);
    }
    return run_current_block(interp);
}

run_stmt_result run_toy_function_expr_list(toy_interp *interp, toy_function *def, const toy_expr_list *args)
{
    switch (def->type) {
    case FUNC_PREDEFINED:
        push_context_pre_def_func(interp, def);
        run_stmt_result res1 = run_predefined_func_expr_list(interp, def->predef, args);
        pop_context(interp);
        if (res1 == REACHED_RETURN) {
            val_assert_valid(&interp->return_val);
        }
        return res1;
    case FUNC_USER_DECLARED:
        push_context_user_def_func(interp, def);
        run_stmt_result res2 = run_user_func_expr_list(interp, &def->code, def->param_names, args);
        pop_context(interp);
        if (res2 == REACHED_RETURN) {
            val_assert_valid(&interp->return_val);
        }
        return res2;
    default:
        invalid_function_type(def->type);
        break;
    }
    assert(0);
    return REACHED_BLOCK_END;
}

toy_val *interp_get_return_value(toy_interp *interp)
{
    toy_val *val = &interp->return_val;
    val_assert_valid(val);
    return val;
}

void interp_set_return_value(toy_interp *interp, toy_val *val)
{
    memcpy(&interp->return_val, val, sizeof(interp->return_val));
}

run_stmt_result run_toy_function_val_list(toy_interp *interp, toy_function *def, const toy_val_list *args)
{
    val_list_assert_valid(args);
    switch (def->type) {
    case FUNC_PREDEFINED:
        push_context_pre_def_func(interp, def);
        run_stmt_result res1 = run_predefined_func_val_list(interp, def->predef, args);
        pop_context(interp);
        return res1;
    case FUNC_USER_DECLARED:
        push_context_user_def_func(interp, def);
        run_stmt_result res2 = run_user_func_val_list(interp, &def->code, def->param_names, args);
        pop_context(interp);
        return res2;
    default:
        invalid_function_type(def->type);
        break;
    }
    assert(0);
    return REACHED_BLOCK_END;
}

run_stmt_result run_toy_function_val(toy_interp *interp, toy_function *def, const toy_val *arg)
{
    toy_val_list func_args = { .val = *arg, .next = NULL };
    return run_toy_function_val_list(interp, def, &func_args);
}

run_stmt_result call_func(toy_interp *interp, toy_str func_name, const toy_expr_list *args)
{
    run_stmt_result res;
    toy_val expr;
    get_result get_res = lookup_identifier(interp, &expr, func_name);
    if (get_res == GET_FOUND) {
        if (expr.type == VAL_FUNC) {
            toy_function *def = expr.func;
            if (def->param_names == &INFINITE_PARAMS) {
                res = run_toy_function_expr_list(interp, def, args);
            } else {
                size_t num_params = str_list_len(def->param_names);
                size_t num_args = expr_list_len(args);
                if (num_args < num_params) {
                    too_few_arguments(num_params, args);
                } else if (num_args > num_params) {
                    too_many_arguments(num_params, args);
                } else {
                    res = run_toy_function_expr_list(interp, def, args);
                }
            }
        } else {
            invalid_operand(EXPR_FUNC_CALL, &expr);
        }
    } else {
        undeclared_identifier(func_name);
    }
    if (res == REACHED_RETURN) {
        val_assert_valid(&interp->return_val);
    }
    return res;
}

static void list_lookup(toy_interp *interp, toy_val *result, toy_val_list *val_list, toy_val *index)
{
    if (index->type == VAL_NUM) {
        if (index->num < 0) {
            invalid_val_list_index(val_list, index->num);
        } else {
            toy_val *lookup_result = val_list_index(val_list, index->num);
            if (lookup_result) {
                *result = *lookup_result;
            } else {
                invalid_val_list_index(val_list, index->num);
            }
        }
    } else {
        invalid_operand(EXPR_COLLECTION_LOOKUP, index);
    }
}

static void map_lookup(toy_interp *interp, toy_val *result, map_val *collection, toy_val *index)
{
    if (index->type == VAL_STR) {
        toy_val *existing_value = map_val_get(collection, index->str);
        if (existing_value) {
            *result = *existing_value;
        } else {
            *result = null_val;
        }
    } else {
        invalid_operand(EXPR_COLLECTION_LOOKUP, index);
    }
}

static void str_lookup(toy_interp *interp, toy_val *result, toy_str collection, toy_val *index)
{
    if (index->type == VAL_NUM) {
        if (index->num >= 0 && index->num < strlen(collection)) {
            result->type = VAL_NUM;
            result->num = collection[(int) index->num];
        } else {
            invalid_string_index(collection, index->num);
        }
    } else {
        invalid_operand(EXPR_COLLECTION_LOOKUP, index);
    }
}

static void collection_lookup(toy_interp *interp, toy_val *result, toy_str identifier, toy_expr *index)
{
    toy_val collection;
    get_result get_res = lookup_identifier(interp, &collection, identifier);
    if (get_res == GET_FOUND) {
        toy_val index_result;
        expr_eval(interp, &index_result, index);    
        if (collection.type == VAL_LIST) {
            list_lookup(interp, result, collection.list, &index_result);
        } else if (collection.type == VAL_MAP) {
            map_lookup(interp, result, collection.map, &index_result);
        } else if (collection.type == VAL_STR) {
            str_lookup(interp, result, collection.str, &index_result);
        } else {
            invalid_operand(EXPR_COLLECTION_LOOKUP, &collection);
        }
    } else {
        undeclared_identifier(identifier);
    }
}

static void op_postfix_decrement(toy_interp *interp, toy_val *result, toy_str id)
{
    toy_val cur_value;
    get_result get_res = lookup_identifier(interp, &cur_value, id);
    if (get_res == GET_FOUND) {
        if (cur_value.type == VAL_NUM) {
            toy_val new_value = { .type = VAL_NUM, .num = cur_value.num - 1 };
            set_variable_value(interp, id, &new_value);
            *result = cur_value;
        } else {
            invalid_operand(EXPR_POSTFIX_DECREMENT, &cur_value);
        }
    } else {
        undeclared_identifier(id);
    }
}

static void op_postfix_increment(toy_interp *interp, toy_val *result, toy_str id)
{
    toy_val cur_value;
    get_result get_res = lookup_identifier(interp, &cur_value, id);
    if (get_res == GET_FOUND) {
        if (cur_value.type == VAL_NUM) {
            toy_val new_value = { .type = VAL_NUM, .num = cur_value.num + 1 };
            set_variable_value(interp, id, &new_value);
            *result = cur_value;
        } else {
            invalid_operand(EXPR_POSTFIX_DECREMENT, &cur_value);
        }
    } else {
        undeclared_identifier(id);
    }
}

static void op_prefix_decrement(toy_interp *interp, toy_val *result, toy_str id)
{
    toy_val cur_value;
    get_result get_res = lookup_identifier(interp, &cur_value, id);
    if (get_res == GET_FOUND) {
        if (cur_value.type == VAL_NUM) {
            toy_val new_value = { .type = VAL_NUM, .num = cur_value.num - 1 };
            set_variable_value(interp, id, &new_value);
            *result = new_value;
        } else {
            invalid_operand(EXPR_POSTFIX_DECREMENT, &cur_value);
        }
    } else {
        undeclared_identifier(id);
    }
}

static void op_prefix_increment(toy_interp *interp, toy_val *result, toy_str id)
{
    toy_val cur_value;
    get_result get_res = lookup_identifier(interp, &cur_value, id);
    if (get_res == GET_FOUND) {
        if (cur_value.type == VAL_NUM) {
            toy_val new_value = { .type = VAL_NUM, .num = cur_value.num + 1 };
            set_variable_value(interp, id, &new_value);
            *result = new_value;
        } else {
            invalid_operand(EXPR_POSTFIX_DECREMENT, &cur_value);
        }
    } else {
        undeclared_identifier(id);
    }
}

typedef struct map_entry_cb_args_struct {
    toy_interp *interp;
    map_val *map;
} map_entry_cb_args;

static item_callback_result map_entry_callback(void *cookie, size_t index, const toy_map_entry_list *list)
{
    map_entry_cb_args *args = (map_entry_cb_args *) cookie;
    const toy_map_entry *map_entry = map_entry_list_payload_const(list);
    toy_val value;
    expr_eval(args->interp, &value, map_entry->value);
    map_val_set(args->map, map_entry->key, &value);
    return CONTINUE_ENUMERATION;
}

static void eval_map(toy_interp *interp, toy_val *result, const toy_map_entry_list *entry_list)
{
    result->type = VAL_MAP;
    result->map = map_val_alloc();
    map_entry_cb_args map_entry_args = { .interp = interp, .map = result->map };
    enumeration_result res = map_entry_list_foreach_const(entry_list, map_entry_callback, &map_entry_args);
    assert(res == ENUMERATION_COMPLETE);
}

void expr_eval(toy_interp *interp, toy_val *result, const toy_expr *expr)
{
    if (!expr) {
        *result = null_val;
        return;
    }
    expr_assert_valid(expr);
    switch (expr->type) {
    case EXPR_AND:
        op_and(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_ASSIGN:
        op_assign(interp, result, expr->assignment.lhs, expr->assignment.rhs);
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
    case EXPR_FIELD_REF:
        op_field_ref(interp, result, expr->field_ref.lhs, expr->field_ref.rhs);
        break;
    case EXPR_FUNC_CALL:
        run_stmt_result res = call_func(interp, expr->func_call.func_name, expr->func_call.args);
        if (res == REACHED_RETURN) {
            *result = *interp_get_return_value(interp);
        } else {
            *result = null_val;
        }
        break;
    case EXPR_GT:
        op_gt(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_GTE:
        op_gte(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_IDENTIFIER:
        get_result get_res = lookup_identifier(interp, result, expr->val.str);
        if (get_res == GET_NOT_FOUND) {
            undeclared_identifier(expr->val.str);
        }
        break;
    case EXPR_IN:
        op_in(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_LIST:
        eval_expr_list(interp, result, expr->list);
        break;
    case EXPR_LITERAL:
        *result = expr->val;
        break;
    case EXPR_LT:
        op_lt(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_LTE:
        op_lte(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_MAP:
        eval_map(interp, result, expr->map);
        break;
    case EXPR_METHOD_CALL:
        op_method_call(interp, result, &expr->method_call);
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
        op_nequal(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_NOT:
        op_not(interp, result, expr->unary_op.arg);
        break;
    case EXPR_OR:
        op_or(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_PLUS:
        op_plus(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_POSTFIX_DECREMENT:
        op_postfix_decrement(interp, result, expr->postfix_decrement.id);
        break;
    case EXPR_POSTFIX_INCREMENT:
        op_postfix_increment(interp, result, expr->postfix_increment.id);
        break;
    case EXPR_PREFIX_DECREMENT:
        op_prefix_decrement(interp, result, expr->prefix_decrement.id);
        break;
    case EXPR_PREFIX_INCREMENT:
        op_prefix_increment(interp, result, expr->prefix_increment.id);
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
    val_assert_valid(result);
}

void invalid_run_stmt_result(run_stmt_result stmt_result)
{
    /* TODO */
    assert(0);
}

toy_bool is_control_result(run_stmt_result stmt_result)
{
    switch (stmt_result) {
    case REACHED_BLOCK_END:
    case EXECUTED_STATEMENT:
        return TOY_FALSE;
    case REACHED_RETURN:
    case REACHED_BREAK:
    case REACHED_CONTINUE:
        return TOY_TRUE;
    default:
        invalid_run_stmt_result(stmt_result);
        break;
    }
    assert(0);
    return TOY_FALSE;
}

/* TODO: Delete me */
run_stmt_result run_one_stmt(toy_interp *interp, toy_stmt *stmt)
{
    return run_stmt(interp, stmt);
}

toy_bool condition_truthy(toy_interp *interp, const toy_expr *expr)
{
    if (expr) {
        toy_val cond_result;
        expr_eval(interp, &cond_result, expr);
        return val_truthy(&cond_result);
    }
    return TOY_TRUE;
}

run_stmt_result run_stmt(toy_interp *interp, const toy_stmt *stmt)
{
    switch (stmt->type) {
    case STMT_BLOCK:
        return block_stmt(interp, &stmt->block_stmt.block);
    case STMT_BREAK:
        return REACHED_BREAK;
    case STMT_CONTINUE:
        return REACHED_CONTINUE;
    case STMT_EXPR:
        toy_val result;
        expr_eval(interp, &result, stmt->expr_stmt.expr);
        /* throw result away */
        return EXECUTED_STATEMENT;
    case STMT_FOR:
        return for_stmt(interp, &stmt->for_stmt);
    case STMT_FUNC_DECL:
        create_function(interp, &stmt->func_decl_stmt.def);
        return EXECUTED_STATEMENT;
    case STMT_IF:
        return if_stmt(interp, &stmt->if_stmt);
    case STMT_NULL:
        return EXECUTED_STATEMENT;
    case STMT_RETURN:
        return return_stmt(interp, &stmt->return_stmt);
    case STMT_VAR_DECL:
        return var_decl_stmt(interp, &stmt->var_decl_stmt);
    case STMT_WHILE:
        return while_stmt(interp, &stmt->while_stmt);
    default:
        invalid_stmt_type(stmt->type);
        break;
    }
    assert(0);
    return EXECUTED_STATEMENT;
}

run_stmt_result run_current_block(toy_interp *interp)
{
    /* TODO: Should use stmt_list_foreach */
    interp_frame_list *frame_list = interp_cur_frame(interp);
    interp_frame *cur_frame = interp_frame_list_payload(frame_list);
    for (; cur_frame->cur_stmt; cur_frame->cur_stmt = cur_frame->cur_stmt->next) {
        run_stmt_result stmt_result;
        stmt_result = run_stmt(interp, &cur_frame->cur_stmt->stmt);
        assert(stmt_result != REACHED_BLOCK_END);
        switch (stmt_result) {
        case EXECUTED_STATEMENT:
            break;
        case REACHED_RETURN:
        case REACHED_BREAK:
        case REACHED_CONTINUE:
            return stmt_result;
        case REACHED_BLOCK_END:
            assert(0); /* shouldn't happen, because not yet at end */
            break;
        default:
            assert(0);
            break;
        }
    }
    /* Reaching the end of a function is equivalent to returning null */
    interp->return_val = null_val;
    return REACHED_BLOCK_END;
}

static run_stmt_result block_stmt(toy_interp *interp, const toy_block *block)
{
    push_context_block_stmt(interp, block);
    run_stmt_result res = run_current_block(interp);
    pop_context(interp);
    if (res == REACHED_BLOCK_END) {
        res = EXECUTED_STATEMENT;
    }
    return res;
}

toy_interp *alloc_interp(const toy_stmt_list *program)
{
    toy_interp *interp;
    interp = mymalloc(toy_interp);
    interp->main_program.type = FUNC_USER_DECLARED;
    interp->main_program.code.stmts = (toy_stmt_list *) program;
    interp->main_program.name = "Top-level";
    interp->main_program.param_names = NULL;
    interp->cur_frame = NULL;
    dump_stack(stderr, "at program start", interp);
    push_context_user_def_func(interp, &interp->main_program);
    return interp;
}

void free_interp(toy_interp *interp)
{
    pop_context(interp);
    while (interp->cur_frame) {
        interp_frame_list *prev = interp->cur_frame->prev;
        interp_frame_free(interp->cur_frame);
        interp->cur_frame = prev;
    }
    free(interp);
}
