#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include "mymalloc.h"
#include "str.h"
#include "log.h"
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
#include "interp-while.h"
#include "interp-frame.h"
#include "predef-function.h"
#include "interp-stack.h"
#include "interp-frame.h"
#include "resolved-name.h"
#include "block.h"
#include "var.h"
#include "list-visitor.h"

#if 0
#define DEBUG_STACK 1
#endif

#if 1
#define DEBUG_INTERP_LOOKUPS
#endif

typedef struct toy_interp_struct {
    const toy_function *main_program;
    interp_stack *stack;
    toy_val return_val;
} toy_interp;

void interp_assert_valid(const toy_interp *interp)
{
    func_assert_valid(interp->main_program);
    val_assert_valid(&interp->return_val);
    interp_stack_assert_valid(interp->stack);
}

static run_stmt_result block_stmt(toy_interp *interp, const toy_block *block);

interp_stack *interp_get_stack(toy_interp *interp)
{
    interp_assert_valid(interp);
    return interp->stack;
}

interp_frame *interp_cur_frame(toy_interp *interp)
{
    interp_assert_valid(interp);
    interp_stack *stack = interp_get_stack(interp);
    interp_stack_assert_valid(stack);
    interp_frame *cur_frame = interp_stack_payload(stack);
    interp_frame_assert_valid(cur_frame);
    return cur_frame;
}

typedef struct expr_list_visitor_struct {
    list_visitor list_vis;
    toy_interp *interp;
    toy_val *result;
} expr_list_visitor;

static item_callback_result append_val_list_callback(expr_list_visitor *expr_list_vis, size_t index, toy_expr_list *expr_list)
{
    toy_expr *expr = expr_list_payload(expr_list);
    toy_interp *interp = expr_list_vis->interp;
    interp_assert_valid(interp);
    toy_val val;
    interp_eval_val(interp, &val, expr);
    toy_val *result = expr_list_vis->result;
    assert(result->type == VAL_LIST);
    if (result->list) {
        result->list = val_list_append(result->list, &val);
    } else {
        result->list = val_list_alloc(&val);
    }
    return CONTINUE_ENUMERATION;
}

static void eval_expr_list(toy_interp *interp, toy_val *result, const toy_expr_list *expr_list)
{
    interp_assert_valid(interp);
    result->type = VAL_LIST;
    result->list = NULL;
    if (expr_list) {
        expr_list_visitor expr_visitor = { .list_vis.visit_entry = (list_entry_visit_func) append_val_list_callback, .interp = interp, .result = result };
        enumeration_result res = list_visitor_visit_list((list_visitor *) &expr_visitor, (generic_list *) expr_list);
        assert(ENUMERATION_COMPLETE == res);
    }
}

static run_stmt_result run_predefined_func_val_list(toy_interp *interp, predefined_func_addr predef)
{
    interp_assert_valid(interp);
    interp_frame *frame = interp_cur_frame(interp);
    assert(FRAME_PRE_DEF_FUNC == frame->type);
    const func_call_frame *func_inv = &frame->func_call;
    return predef(interp, func_inv->arguments, func_inv->num_arguments);
}

run_stmt_result interp_run_func_expr_list(toy_interp *interp, const toy_function *func, const toy_expr_list *args)
{
    interp_assert_valid(interp);
    toy_val actual_args;
    eval_expr_list(interp, &actual_args, args);
    assert(VAL_LIST == actual_args.type);
    switch (func->type) {
    case FUNC_PREDEFINED:
        interp->stack = interp_stack_push_predef_func(interp->stack, func, actual_args.list);
        run_stmt_result res1 = run_predefined_func_val_list(interp, func->predef);
        interp->stack = interp_stack_pop(interp->stack);
        if (res1 == REACHED_RETURN) {
            val_assert_valid(&interp->return_val);
        }
        return res1;
    case FUNC_USER_DECLARED:
        interp->stack = interp_stack_push_user_func(interp->stack, func, actual_args.list);
        run_stmt_result res2 = interp_run_current_block(interp);
        interp->stack = interp_stack_pop(interp->stack);
        if (res2 == REACHED_RETURN) {
            val_assert_valid(&interp->return_val);
        }
        return res2;
    default:
        invalid_function_type(func->type);
        break;
    }
    assert(0);
    return REACHED_BLOCK_END;
}

toy_val *interp_get_return_value(toy_interp *interp)
{
    interp_assert_valid(interp);
    toy_val *val = &interp->return_val;
    val_assert_valid(val);
    return val;
}

void interp_set_return_value(toy_interp *interp, toy_val *val)
{
    interp_assert_valid(interp);
    val_assert_valid(val);
    interp->return_val = *val;
}

run_stmt_result interp_run_func_val_list(toy_interp *interp, toy_function *def, const toy_val_list *args)
{
    interp_assert_valid(interp);
    val_list_assert_valid(args);
    switch (def->type) {
    case FUNC_PREDEFINED:
        interp->stack = interp_stack_push_predef_func(interp->stack, def, args);
        run_stmt_result res1 = run_predefined_func_val_list(interp, def->predef);
        interp->stack = interp_stack_pop(interp->stack);
        return res1;
    case FUNC_USER_DECLARED:
        interp->stack = interp_stack_push_user_func(interp->stack, def, args);
        run_stmt_result res2 = interp_run_current_block(interp);
        interp->stack = interp_stack_pop(interp->stack);
        return res2;
    default:
        invalid_function_type(def->type);
        break;
    }
    assert(0);
    return REACHED_BLOCK_END;
}

run_stmt_result interp_run_func_single_arg(toy_interp *interp, toy_function *def, const toy_val *arg)
{
    interp_assert_valid(interp);
    toy_val_list func_args = { .val = *arg, .next = NULL };
    return interp_run_func_val_list(interp, def, &func_args);
}

run_stmt_result interp_call_func(toy_interp *interp, const toy_function *func, const toy_expr_list *args)
{
    interp_assert_valid(interp);
    func_assert_valid(func);
    run_stmt_result res;
    if (func->param_names == &INFINITE_PARAMS) {
        res = interp_run_func_expr_list(interp, func, args);
    } else {
        size_t num_params = str_list_len(func->param_names);
        size_t num_args = expr_list_len(args);
        if (num_args < num_params) {
            too_few_arguments(num_params, args);
        } else if (num_args > num_params) {
            too_many_arguments(num_params, args);
        } else {
            res = interp_run_func_expr_list(interp, func, args);
        }
    }
    if (res == REACHED_RETURN) {
        val_assert_valid(&interp->return_val);
    }
    return res;
}

static void list_lookup(toy_interp *interp, toy_val *result, const toy_val_list *list, toy_val *index)
{
    interp_assert_valid(interp);
    if (index->type == VAL_NUM) {
        if (index->num < 0) {
            invalid_val_list_index(list, index->num);
        } else {
            const toy_val *lookup_result = val_list_index_const(list, index->num);
            if (lookup_result) {
                *result = *lookup_result;
            } else {
                invalid_val_list_index(list, index->num);
            }
        }
    } else {
        invalid_operand(EXPR_COLLECTION_LOOKUP, index);
    }
}

static void map_lookup(toy_interp *interp, toy_val *result, const map_val *map, toy_val *index)
{
    interp_assert_valid(interp);
    if (index->type == VAL_STR) {
        const toy_val *retrieved_value = map_val_get_const(map, index->str);
        if (retrieved_value) {
            *result = *retrieved_value;
        } else {
            *result = null_val;
        }
    } else {
        invalid_operand(EXPR_COLLECTION_LOOKUP, index);
    }
}

static void str_lookup(toy_interp *interp, toy_val *result, toy_str str, toy_val *index)
{
    interp_assert_valid(interp);
    if (index->type == VAL_NUM) {
        if (index->num >= 0 && index->num < strlen(str)) {
            result->type = VAL_NUM;
            result->num = str[(int) index->num];
        } else {
            invalid_string_index(str, index->num);
        }
    } else {
        invalid_operand(EXPR_COLLECTION_LOOKUP, index);
    }
}

static void collection_lookup(toy_interp *interp, toy_val *result, toy_identifier *collection_identifier, toy_expr *index)
{
    interp_assert_valid(interp);
    toy_val index_result;
    interp_eval_val(interp, &index_result, index);
    /* type checking of index is delegated to collection-type-specific subroutines */
    const toy_val *collection_val = interp_get_rvalue(interp, collection_identifier);
    if (collection_val->type == VAL_LIST) {
        list_lookup(interp, result, collection_val->list, &index_result);
    } else if (collection_val->type == VAL_MAP) {
        map_lookup(interp, result, collection_val->map, &index_result);
    } else if (collection_val->type == VAL_STR) {
        str_lookup(interp, result, collection_val->str, &index_result);
    } else {
        invalid_operand(EXPR_COLLECTION_LOOKUP, collection_val);
    }
}

static toy_var *interp_get_param_closure(toy_interp *interp, const closure *param_ref)
{
    interp_assert_valid(interp);
    interp_stack *stack = interp_get_stack(interp);
    interp_frame *frame = interp_stack_index(stack, param_ref->frames_up);
    toy_var *var = interp_frame_get_func_arg(frame, param_ref->var_index);
    var_assert_valid(var);
    return var;
}

static toy_var *interp_get_var_closure(toy_interp *interp, const closure *closure)
{
    interp_assert_valid(interp);
    interp_stack *stack = interp_get_stack(interp);
    /* FIXME: This is indexing into the lexical scope, but then into the runtime variables' values */
    interp_frame *frame = interp_stack_index(stack, closure->frames_up);
    toy_var *var = interp_frame_get_var(frame, closure->var_index);
    var_assert_valid(var);
#ifdef DEBUG_INTERP_LOOKUPS
    log_printf("interp: var retrieved: ");
    var_dump(var, TOY_FALSE);
    log_putc('\n');
#endif /* DEBUG_INTERP_LOOKUPS */
    return var;
}

toy_var *interp_get_lvalue(toy_interp *interp, toy_identifier *identifier)
{
    interp_assert_valid(interp);
#ifdef DEBUG_INTERP_LOOKUPS
    log_debug("interp: retrieving lvalue");
    resolved_name_dump(&identifier->resolved);
#endif /* DEBUG_INTERP_LOOKUPS */

    switch (identifier->resolved.type) {
    case REF_FUNC_DECL:
        /* Function declarations are immutable */
        invalid_lvalue(&identifier->resolved);
        break;
    case REF_FUNC_PARAM:
        closure *param_ref = &identifier->resolved.func_param;
        log_debug("interp: lvalue is func param");
        toy_var *param_var = interp_get_param_closure(interp, param_ref);
        var_assert_valid(param_var);
        return param_var;
    case REF_PREDEF_CONST:
    case REF_PREDEF_FUNC:
        log_debug("interp: lvalue is predef const or func");
        invalid_lvalue(&identifier->resolved);
        break;
    case REF_UNDEFINED:
        log_printf("\nUndefined reference to name '%s'\n", identifier->name);
        assert(0);
        break;
    case REF_VAR_DECL:
        closure *var_ref = &identifier->resolved.var_decl;
        toy_var *var_decl_var = interp_get_var_closure(interp, var_ref);
        var_assert_valid(var_decl_var);
        return var_decl_var;
    default:
        assert(0);
        break;
    }
    assert(0);
    return NULL;
}

const toy_val *interp_get_rvalue(toy_interp *interp, const toy_identifier *identifier)
{
    interp_assert_valid(interp);
#ifdef DEBUG_INTERP_LOOKUPS
    log_debug("interp: retrieving rvalue");
    resolved_name_dump(&identifier->resolved);
#endif /* DEBUG_INTERP_LOOKUPS */

    switch (identifier->resolved.type) {
    case REF_FUNC_DECL:
        const toy_func_decl_stmt *func_decl_stmt = identifier->resolved.func_decl_stmt;
        toy_val *func_val = func_decl_stmt->val;
        assert(VAL_FUNC == func_val->type);
        val_assert_valid(func_val);
        return func_val;
    case REF_FUNC_PARAM:
        const closure *param_ref = &identifier->resolved.func_param;
        toy_var *func_param_var = interp_get_param_closure(interp, param_ref);
        toy_val *func_param_val = var_get(func_param_var);
        val_assert_valid(func_param_val);
        return func_param_val;
    case REF_PREDEF_CONST:
        const predefined_constant *predef_const = identifier->resolved.predef_const;
        const toy_val *predef_const_val = &predef_const->value;
        val_assert_valid(predef_const_val);
        return predef_const_val;
    case REF_PREDEF_FUNC:
        const toy_val *predef_func_val = identifier->resolved.predef_func;
        val_assert_valid(predef_func_val);
        return predef_func_val;
    case REF_UNDEFINED:
        assert(0);
        break;
    case REF_VAR_DECL:
        const closure *var_ref = &identifier->resolved.var_decl;
        toy_var *var_decl_var = interp_get_var_closure(interp, var_ref);
        toy_val *var_val = var_get(var_decl_var);
        val_assert_valid(var_val);
        return var_val;
    default:
        assert(0);
        break;
    }
}

static void op_postfix_decrement(toy_interp *interp, toy_val *result, toy_identifier *identifier)
{
    interp_assert_valid(interp);
    toy_var *var = interp_get_lvalue(interp, identifier);
    toy_val *val = var_get(var);
    if (val->type == VAL_NUM) {
        *result = *val;
        val->num--;
    } else {
        invalid_operand(EXPR_POSTFIX_DECREMENT, val);
    }
}

static void op_postfix_increment(toy_interp *interp, toy_val *result, toy_identifier *identifier)
{
    interp_assert_valid(interp);
    toy_var *var = interp_get_lvalue(interp, identifier);
    toy_val *val = var_get(var);
    if (val->type == VAL_NUM) {
        *result = *val;
        val->num++;
    } else {
        invalid_operand(EXPR_POSTFIX_INCREMENT, val);
    }
}

static void op_prefix_decrement(toy_interp *interp, toy_val *result, toy_identifier *identifier)
{
    interp_assert_valid(interp);
    toy_var *var = interp_get_lvalue(interp, identifier);
    toy_val *val = var_get(var);
    if (val->type == VAL_NUM) {
        val->num--;
        *result = *val;
    } else {
        invalid_operand(EXPR_POSTFIX_DECREMENT, val);
    }
}

static void op_prefix_increment(toy_interp *interp, toy_val *result, toy_identifier *identifier)
{
    interp_assert_valid(interp);
    toy_var *var = interp_get_lvalue(interp, identifier);
    toy_val *val = var_get(var);
    if (val->type == VAL_NUM) {
        val->num++;
        *result = *val;
    } else {
        invalid_operand(EXPR_POSTFIX_INCREMENT, val);
    }
}

typedef struct map_entry_visitor_struct {
    list_visitor list_vis;
    toy_interp *interp;
    map_val *map;
} map_entry_visitor;

static item_callback_result map_entry_callback(map_entry_visitor *map_entry_vis, size_t index, const toy_map_expr_entry_list *list)
{
    const toy_map_expr_entry *map_entry = map_expr_entry_list_payload_const(list);
    toy_val value;
    interp_assert_valid(map_entry_vis->interp);
    interp_eval_val(map_entry_vis->interp, &value, map_entry->expr);
    if (!map_entry_vis->map) {
        map_entry_vis->map = map_val_alloc();
    }
    set_result set_res = map_val_set(map_entry_vis->map, map_entry->key, &value);
    assert(SET_NEW == set_res);
    return CONTINUE_ENUMERATION;
}

static void eval_map(toy_interp *interp, toy_val *result, const toy_map_expr_entry_list *entry_list)
{
    interp_assert_valid(interp);
    result->type = VAL_MAP;
    result->map = NULL;
    map_entry_visitor map_entry_args = {
        .list_vis.visit_entry = (list_entry_visit_func) map_entry_callback,
        .interp = interp,
        .map = result->map
    };
    enumeration_result res = list_visitor_visit_list((list_visitor *) &map_entry_args, (generic_list *) entry_list);
    assert(res == ENUMERATION_COMPLETE);
}

static void op_func_call(toy_interp *interp, toy_val *result, toy_func_call *call)
{
    interp_assert_valid(interp);
    const toy_val *referenced_val = interp_get_rvalue(interp, &call->id);
    if (VAL_FUNC == referenced_val->type) {
        const toy_function *func = referenced_val->func;
        run_stmt_result res = interp_call_func(interp, func, call->args);
        if (res == REACHED_RETURN) {
            *result = *interp_get_return_value(interp);
        } else {
            *result = null_val;
        }
    } else {
        invalid_operand(EXPR_FUNC_CALL, referenced_val);
    }
}

void interp_eval_var(toy_interp *interp, toy_var *result, toy_expr *expr)
{
    interp_assert_valid(interp);
    expr_assert_valid(expr);
    toy_val *val = var_get(result);
    interp_eval_val(interp, val, expr);
    val_assert_valid(val);
    val_assert_valid(val);
}

static void expr_identifier(toy_interp *interp, toy_val *result, toy_identifier *identifier)
{
    interp_assert_valid(interp);
    toy_var *referenced_var = interp_get_lvalue(interp, identifier);
    var_assert_valid(referenced_var);
    toy_val *referenced_val = var_get(referenced_var);
    val_assert_valid(referenced_val);
    *result = *referenced_val;
    val_assert_valid(result);
}

void interp_eval_val(toy_interp *interp, toy_val *result, toy_expr *expr)
{
    interp_assert_valid(interp);
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
        op_assign(interp, result, &expr->assignment.id, expr->assignment.rhs);
        break;
    case EXPR_COLLECTION_LOOKUP:
        collection_lookup(interp, result, &expr->collection_lookup.id, expr->collection_lookup.rhs);
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
        op_field_ref(interp, result, &expr->field_ref.id, expr->field_ref.field_name);
        break;
    case EXPR_FUNC_CALL:
        op_func_call(interp, result, &expr->func_call);
        break;
    case EXPR_GT:
        op_gt(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_GTE:
        op_gte(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_IDENTIFIER:
        expr_identifier(interp, result, &expr->id);
        break;
    case EXPR_IN:
        op_in(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_LIST:
        eval_expr_list(interp, result, expr->expr_list);
        break;
    case EXPR_LITERAL:
        assert(expr->val->type != VAL_FUNC);
        *result = *expr->val;
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
        op_postfix_decrement(interp, result, &expr->postfix_decrement.id);
        break;
    case EXPR_POSTFIX_INCREMENT:
        op_postfix_increment(interp, result, &expr->postfix_increment.id);
        break;
    case EXPR_PREFIX_DECREMENT:
        op_prefix_decrement(interp, result, &expr->prefix_decrement.id);
        break;
    case EXPR_PREFIX_INCREMENT:
        op_prefix_increment(interp, result, &expr->prefix_increment.id);
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

toy_bool interp_is_control_result(run_stmt_result stmt_result)
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
run_stmt_result interp_run_one_stmt(toy_interp *interp, toy_stmt *stmt)
{
    interp_assert_valid(interp);
    return interp_run_stmt(interp, stmt);
}

toy_bool interp_condition_truthy(toy_interp *interp, toy_expr *expr)
{
    interp_assert_valid(interp);
    if (expr) {
        toy_val cond_result;
        interp_eval_val(interp, &cond_result, expr);
        return val_truthy(&cond_result);
    }
    /* An empty condition is treated as true, so we can do eg for(;;) {...} */
    return TOY_TRUE;
}

#define DEBUG_VARIABLES

typedef struct var_decl_visitor_struct {
    list_visitor list_vis;
    toy_interp *interp;
} var_decl_visitor;

static item_callback_result set_var_callback(var_decl_visitor *var_decl_vis, size_t index, const toy_var_decl_list *item)
{
    const toy_var_decl *var_decl = var_decl_list_payload_const(item);
    toy_interp *interp = var_decl_vis->interp;
    interp_assert_valid(interp);
    interp_frame *cur_frame = interp_cur_frame(interp);
    interp_frame_assert_valid(cur_frame);
    toy_val *initial_val = mymalloc(toy_val);
    interp_eval_val(interp, initial_val, var_decl->value);

#ifdef DEBUG_VARIABLES
    log_printf("interp: setting frame var %d to initial val ", var_decl->decl_index);
    val_dump(initial_val, 0);
    log_putc('\n');
#endif /* DEBUG_VARIABLES */

    toy_var *var = interp_frame_get_var(cur_frame, var_decl->decl_index);
    var_init(var);
    var_set(var, initial_val);

    return CONTINUE_ENUMERATION;
}

static run_stmt_result var_decl_stmt(toy_interp *interp, const toy_var_decl_stmt *var_decl_stmt)
{
    interp_assert_valid(interp);
    var_decl_visitor var_decl_vis = { .list_vis.visit_entry = (list_entry_visit_func) set_var_callback, .interp = interp };
    toy_var_decl_list *var_decl_list = var_decl_stmt->var_decl_list;
    enumeration_result res = list_visitor_visit_list((list_visitor *) &var_decl_vis, (generic_list *) var_decl_list);
    assert(ENUMERATION_COMPLETE == res);
    interp_assert_valid(interp);
    return EXECUTED_STATEMENT;
}

static run_stmt_result func_decl_stmt(toy_interp *interp, const toy_func_decl_stmt *func_decl)
{
    interp_assert_valid(interp);
    interp_stack *stack = interp->stack;
    interp_frame *cur_frame = interp_stack_payload(stack);
    assert(func_decl->val->type == VAL_FUNC);
    assert(func_decl->val->func == func_decl->func);
#ifdef DEBUG_VARIABLES
    log_printf("interp: setting frame var %d to initial function ", func_decl->decl_index);
    val_dump(func_decl->val, 0);
    log_printf("\n");
#endif /* DEBUG_VARIABLES */
    toy_var *var = interp_frame_get_var(cur_frame, func_decl->decl_index);
    var_init(var);
    var_set(var, func_decl->val);
    interp_assert_valid(interp);
    return EXECUTED_STATEMENT;
}

run_stmt_result interp_run_stmt(toy_interp *interp, const toy_stmt *stmt)
{
    interp_assert_valid(interp);
    switch (stmt->type) {
    case STMT_BLOCK:
        return block_stmt(interp, stmt->block_stmt.block);
    case STMT_BREAK:
        return REACHED_BREAK;
    case STMT_CONTINUE:
        return REACHED_CONTINUE;
    case STMT_EXPR:
        toy_val result;
        interp_eval_val(interp, &result, stmt->expr_stmt.expr);
        /* throw result away */
        return EXECUTED_STATEMENT;
    case STMT_FOR:
        return for_stmt(interp, &stmt->for_stmt);
    case STMT_FUNC_DECL:
        return func_decl_stmt(interp, &stmt->func_decl_stmt);
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

typedef struct stmt_run_visitor_struct {
    list_visitor list_vis;
    toy_interp *interp;
    run_stmt_result run_result;
} stmt_run_visitor;

static item_callback_result stmt_run_callback(stmt_run_visitor *stmt_run_vis, size_t index, const toy_stmt_list *item)
{
    const toy_stmt *stmt = stmt_list_payload_const(item);
    toy_interp *interp = stmt_run_vis->interp; 
    interp_assert_valid(interp);
    stmt_run_vis->run_result = interp_run_stmt(interp, stmt);
    assert(stmt_run_vis->run_result != REACHED_BLOCK_END);
    switch (stmt_run_vis->run_result) {
    case EXECUTED_STATEMENT:
        break;
    case REACHED_RETURN:
    case REACHED_BREAK:
    case REACHED_CONTINUE:
        return STOP_ENUMERATION;
    case REACHED_BLOCK_END:
        assert(0); /* shouldn't happen, because not yet at end */
        break;
    default:
        assert(0);
        break;
    }
    return CONTINUE_ENUMERATION;
}

run_stmt_result interp_run_current_block(toy_interp *interp)
{
    interp_assert_valid(interp);
    interp_frame *cur_frame = interp_cur_frame(interp);
    interp_frame_assert_valid(cur_frame);
    toy_stmt_list *cur_stmt = cur_frame->cur_stmt;
    stmt_list_assert_valid(cur_stmt);
    stmt_run_visitor stmt_run_vis = {
        .list_vis.visit_entry = (list_entry_visit_func) stmt_run_callback,
        .interp = interp
    };
    enumeration_result res = list_visitor_visit_list((list_visitor *) &stmt_run_vis, (generic_list *) cur_stmt);
    if (ENUMERATION_COMPLETE == res) {
        /* Reaching the end of a function is equivalent to returning null */
        interp->return_val = null_val;
        stmt_run_vis.run_result = REACHED_BLOCK_END;
    }
    return stmt_run_vis.run_result;
}

static run_stmt_result block_stmt(toy_interp *interp, const toy_block *block)
{
    interp_assert_valid(interp);
    interp->stack = interp_stack_push_block(interp->stack, block);
    run_stmt_result res = interp_run_current_block(interp);
    interp->stack = interp_stack_pop(interp->stack);
    if (res == REACHED_BLOCK_END) {
        res = EXECUTED_STATEMENT;
    }
    return res;
}

toy_interp *interp_alloc(const toy_function *program)
{
    toy_interp *interp;
    interp = mymalloc(toy_interp);
    interp->main_program = program;
    interp->stack = interp_stack_push_user_func(interp->stack, interp->main_program, NULL);
    /* interp_stack_dump("at program start", interp->stack); */
    interp_assert_valid(interp);
    return interp;
}

void interp_free(toy_interp *interp)
{
    interp_assert_valid(interp);
    interp->stack = interp_stack_pop(interp->stack);
    interp_stack_free(interp->stack);
    free(interp);
}

/* TODO: Can these be unified? */
void interp_push_if(toy_interp *interp, const toy_block *block)
{
    interp_assert_valid(interp);
    interp->stack = interp_stack_push_if(interp->stack, block);
}

void interp_push_loop(toy_interp *interp, const toy_block *block)
{
    interp_assert_valid(interp);
    interp->stack = interp_stack_push_loop(interp->stack, block);
}

void interp_pop(toy_interp *interp)
{
    interp_assert_valid(interp);
    interp->stack = interp_stack_pop(interp->stack);
}
