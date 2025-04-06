#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include "mymalloc.h"
#include "interp.h"
#include "toy-val-list.h"
#include "toy-str-list.h"
#include "toy-expr-list.h"
#include "toy-map.h"
#include "util.h"
#include "operations.h"
#include "constants.h"
#include "functions.h"
#include "errors.h"

typedef struct toy_interp_struct {
    toy_block top_block;
    toy_map *symbols;
    /* TODO: Should be a stack */
    const toy_block *cur_block;
    toy_val return_val;
} toy_interp;

static enum run_stmt_result run_block(toy_interp *interp, const toy_block *block);

toy_bool convert_to_bool(const toy_val *val)
{
    switch (val->type) {
    case VAL_BOOL:
        return val->boolean;
    case VAL_NULL:
        return 0;
    case VAL_NUM:
        return val->num != 0;
    case VAL_LIST:
        return val_list_len(val->list) != 0;
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

#if 0
static toy_bool convert_to_num(const toy_val *val)
{
    switch (val->type) {
    case VAL_BOOL:
        return val->boolean ? TOY_TRUE : TOY_FALSE;
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
#endif

static void push_context(toy_interp *interp, const toy_block *block)
{
    /* TODO: This needs to be a stack */
    interp->cur_block = block;
}

static void pop_context(toy_interp *interp)
{
    /* TODO: context stack */
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

int lookup_identifier(toy_interp *interp, toy_val *result, const toy_str name)
{
    const toy_val *predef_const = lookup_predefined_constant(name);
    if (predef_const) {
        *result = *((toy_val *) predef_const);
        return 1;
    }
    const toy_func_def *predef_func = lookup_predefined_function(name);
    if (predef_func) {
        result->type = VAL_FUNC;
        result->func = (toy_func_def *) predef_func;
        return 1;
    }
    toy_val *existing_value = map_get(interp->symbols, name);
    if (existing_value) {
        *result = *existing_value;
        return 1;
    }
    return 0;
}

enum set_variable_policy_enum {
    POLICY_MUST_ALREADY_EXIST,
    POLICY_MUST_NOT_ALREADY_EXIST
};
typedef enum set_variable_policy_enum set_variable_policy;

/* TODO: Move these into symbol-table.c */
static int set_variable_value_policy(toy_interp *interp, const toy_str name, const toy_val *value, set_variable_policy policy)
{
    if (is_predefined(name)) {
        readonly_identifier(name);
    }
    toy_val old_value;
    int already_exists = lookup_identifier(interp, &old_value, name);
    switch (policy) {
    case POLICY_MUST_ALREADY_EXIST:
        if (!already_exists) {
            undeclared_identifier(name);
        }
        break;
    case POLICY_MUST_NOT_ALREADY_EXIST:
        if (already_exists) {
            duplicate_identifier(name);
        }
        break;
    default:
        assert(0);
        break;
    }
    int added_new = map_set(interp->symbols, name, value);
    return added_new;
}

static void set_variable_value(toy_interp *interp, const toy_str name, const toy_val *value)
{
    int added_new = set_variable_value_policy(interp, name, value, POLICY_MUST_ALREADY_EXIST);
    assert(!added_new);
}

static void create_variable_value(toy_interp *interp, const toy_str name, const toy_val *value)
{
    int added_new = set_variable_value_policy(interp, name, value, POLICY_MUST_NOT_ALREADY_EXIST);
    assert(added_new);
}

static void create_variable_expr(toy_interp *interp, const toy_str name, const toy_expr *value)
{
    toy_val value_result;
    eval_expr(interp, &value_result, value);
    create_variable_value(interp, name, &value_result);
}

static void create_function(toy_interp *interp, const toy_func_def *def)
{
    if (is_predefined(def->name)) {
        readonly_identifier(def->name);
    }
    toy_val old_value;
    int already_exists = lookup_identifier(interp, &old_value, def->name);
    if (already_exists) {
        duplicate_identifier(def->name);
    }
    /* TODO: Remove cast below using const in .func member? Beware const poisoning... */
    toy_val func_val = { .type = VAL_FUNC, .func = (toy_func_def *) def };
    map_set(interp->symbols, def->name, &func_val);
}

static void op_assign(toy_interp *interp, toy_val *result, toy_str name, toy_expr *arg)
{
    toy_val value_result;
    eval_expr(interp, &value_result, arg);
    set_variable_value(interp, name, &value_result);
    *result = value_result;
}

static void eval_list(toy_interp *interp, toy_val *result, const toy_expr_list *expr_list)
{
    result->type = VAL_LIST;
    if (expr_list) {
        toy_val element;
        eval_expr(interp, &element, expr_list->expr);
        result->list = alloc_val_list_own(&element);
        for (expr_list = expr_list->next; expr_list; expr_list = expr_list->next) {
            eval_expr(interp, &element, expr_list->expr);
            append_val_list_own(result->list, &element);
        }
    } else {
        result->list = NULL;
    }
}

static void run_predefined_func_val_list(toy_interp *interp, toy_val *result, predefined_func_addr predef, toy_val_list *args)
{
    push_context(interp, NULL);
    predef(interp, result, args);
    pop_context(interp);
}

static void run_predefined_func_expr_list(toy_interp *interp, toy_val *result, predefined_func_addr predef, toy_expr_list *arg_exprs)
{
    toy_val args;
    eval_list(interp, &args, arg_exprs);
    assert(VAL_LIST == args.type);
    run_predefined_func_val_list(interp, result, predef, args.list);
}

static void run_user_func_val_list(toy_interp *interp, toy_val *result, toy_block *block, toy_str_list *param_names, toy_val_list *args)
{
    push_context(interp, block);
    for (toy_str_list *param_name = param_names; param_name && args; param_name = param_name->next, args = args->next) {
        create_variable_value(interp, param_name->str, args->val);
    }
    run_current_block(interp);
    *result = interp->return_val;
    pop_context(interp);
    *result = null_val;
}

static void run_user_func_expr_list(toy_interp *interp, toy_val *result, toy_block *block, toy_str_list *param_names, toy_expr_list *args)
{
    push_context(interp, block);
    for (toy_str_list *param_name = param_names; param_name && args; param_name = param_name->next, args = args->next) {
        create_variable_expr(interp, param_name->str, args->expr);
    }
    run_current_block(interp);
    *result = interp->return_val;
    pop_context(interp);
    *result = null_val;
}

void run_toy_function_expr_list(toy_interp *interp, toy_val *result, toy_func_def *def, toy_expr_list *args)
{
    switch (def->type) {
    case FUNC_PREDEFINED:
        run_predefined_func_expr_list(interp, result, def->predef, args);
        break;
    case FUNC_USER_DECLARED:
        run_user_func_expr_list(interp, result, &def->code, def->param_names, args);
        break;
    default:
        invalid_function_type(def->type);
        break;
    }
}

void run_toy_function_val_list(toy_interp *interp, toy_val *result, toy_func_def *def, toy_val_list *args)
{
    switch (def->type) {
    case FUNC_PREDEFINED:
        run_predefined_func_val_list(interp, result, def->predef, args);
        break;
    case FUNC_USER_DECLARED:
        run_user_func_val_list(interp, result, &def->code, def->param_names, args);
        break;
    default:
        invalid_function_type(def->type);
        break;
    }
}

void call_func(toy_interp *interp, toy_val *result, toy_str func_name, toy_expr_list *args)
{
    toy_val expr;
    int already_exists = lookup_identifier(interp, &expr, func_name);
    if (already_exists) {
        if (expr.type == VAL_FUNC) {
            toy_func_def *def = expr.func;
            if (def->param_names == &INFINITE_PARAMS) {
                run_toy_function_expr_list(interp, result, def, args);
            } else {
                size_t num_params = str_list_len(def->param_names);
                size_t num_args = expr_list_len(args);
                if (num_args < num_params) {
                    too_few_arguments(num_params, args);
                } else if (num_args > num_params) {
                    too_many_arguments(num_params, args);
                } else {
                    run_toy_function_expr_list(interp, result, def, args);
                }
            }
        } else {
            invalid_operand(EXPR_FUNC_CALL, &expr);
        }
    } else {
        undeclared_identifier(func_name);
    }
}

static void list_lookup(toy_interp *interp, toy_val *result, toy_val_list *collection, toy_expr *index)
{
    toy_val index_result;
    eval_expr(interp, &index_result, index);
    if (index_result.type == VAL_NUM) {
        toy_val *val;
        val = val_list_index(collection, index_result.num);
        *result = *val;
    } else {
        invalid_operand(EXPR_COLLECTION_LOOKUP, &index_result);
    }
}

static void map_lookup(toy_interp *interp, toy_val *result, toy_map *collection, toy_expr *index)
{
    toy_val index_result;
    eval_expr(interp, &index_result, index);
    if (index_result.type == VAL_STR) {
        toy_val *existing_value = map_get(collection, index_result.str);
        if (existing_value) {
            *result = *existing_value;
        } else {
            *result = null_val;
        }
    } else {
        invalid_operand(EXPR_COLLECTION_LOOKUP, &index_result);
    }
}

static void str_lookup(toy_interp *interp, toy_val *result, toy_str collection, toy_expr *index)
{
    toy_val index_result;
    eval_expr(interp, &index_result, index);
    if (index_result.type == VAL_NUM) {
        if (index_result.num >= 0 && index_result.num < strlen(collection)) {
            result->type = VAL_STR;
            /* TODO: Hide string memory management */
            result->str = malloc(2);
            result->str[0] = collection[(int) index_result.num];
            result->str[1] = '\0';
        } else {
            invalid_string_index(collection, index_result.num);
        }
    } else {
        invalid_operand(EXPR_COLLECTION_LOOKUP, &index_result);
    }
}

static void collection_lookup(toy_interp *interp, toy_val *result, toy_str identifier, toy_expr *index)
{
    toy_val collection;
    int already_exists = lookup_identifier(interp, &collection, identifier);
    if (already_exists) {
        if (collection.type == VAL_LIST) {
            list_lookup(interp, result, collection.list, index);
        } else if (collection.type == VAL_MAP) {
            map_lookup(interp, result, collection.map, index);
        } else if (collection.type == VAL_STR) {
            str_lookup(interp, result, collection.str, index);
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
    int found = lookup_identifier(interp, &cur_value, id);
    if (found) {
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
    int found = lookup_identifier(interp, &cur_value, id);
    if (found) {
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
    int found = lookup_identifier(interp, &cur_value, id);
    if (found) {
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
    int found = lookup_identifier(interp, &cur_value, id);
    if (found) {
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

static void eval_map(toy_interp *interp, toy_val *result, const toy_map_entry_list *entry_list)
{
    result->type = VAL_MAP;
    result->map = alloc_map();
    for (; entry_list; entry_list = entry_list->next) {
        toy_val value;
        eval_expr(interp, &value, entry_list->value);
        map_set(result->map, entry_list->key, &value);
    }
}

void eval_expr(toy_interp *interp, toy_val *result, const toy_expr *expr)
{
    if (!expr) {
        *result = null_val;
        return;
    }
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
        call_func(interp, result, expr->func_call.func_name, expr->func_call.args);
        break;
    case EXPR_GT:
        op_gt(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_GTE:
        op_gte(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_IDENTIFIER:
        int found = lookup_identifier(interp, result, expr->val.str);
        if (!found) {
            undeclared_identifier(expr->val.str);
        }
        break;
    case EXPR_IN:
        op_in(interp, result, expr->binary_op.arg1, expr->binary_op.arg2);
        break;
    case EXPR_LIST:
        eval_list(interp, result, expr->list);
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
}

static void for_stmt(toy_interp *interp, const toy_for_stmt *for_stmt) {
    if (for_stmt->at_start) {
        assert(!for_stmt->at_start->next);
        run_stmt(interp, for_stmt->at_start);
    }
    for (;;) {
        if (for_stmt->condition) {
            toy_val cond_result;
            eval_expr(interp, &cond_result, for_stmt->condition);
            if (cond_result.type != VAL_BOOL) {
                invalid_argument_type(VAL_BOOL, &cond_result);
            }
            assert(VAL_BOOL == cond_result.type);
            if (!cond_result.boolean) {
                break;
            }
        }
        run_block(interp, &for_stmt->body);
        if (for_stmt->at_end) {
            assert(!for_stmt->at_end->next);
            run_stmt(interp, for_stmt->at_end);
        }
    }
}

static void if_stmt(toy_interp *interp, const toy_if_stmt *if_stmt)
{
    unsigned int found_one = 0;
    for (toy_if_arm *arm = if_stmt->arms; arm; arm = arm->next) {
        toy_val cond_result;
        eval_expr(interp, &cond_result, arm->condition);
        if (VAL_BOOL != cond_result.type) {
            invalid_argument_type(VAL_BOOL, &cond_result);
        }
        assert(VAL_BOOL == cond_result.type);
        if (cond_result.boolean) {
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
        toy_val cond_result;
        eval_expr(interp, &cond_result, while_stmt->condition);
        if (VAL_BOOL != cond_result.type) {
            invalid_operand(VAL_BOOL, &cond_result);
        }
        assert(VAL_BOOL == cond_result.type);
        if (cond_result.boolean) {
            break;
        }
        run_block(interp, &while_stmt->body);
    }
}

static void return_stmt(toy_interp *interp, const toy_return_stmt *return_stmt)
{
    eval_expr(interp, &interp->return_val, return_stmt->expr);
}

enum run_stmt_result run_stmt(toy_interp *interp, const toy_stmt *stmt)
{
    switch (stmt->type) {
    case STMT_BLOCK:
        run_block(interp, &stmt->block_stmt.block);
        break;
    case STMT_BREAK:
        return REACHED_BREAK;
    case STMT_CONTINUE:
        return REACHED_CONTINUE;
    case STMT_EXPR:
        toy_val result;
        eval_expr(interp, &result, stmt->expr_stmt.expr);
        break;
    case STMT_FOR:
        for_stmt(interp, &stmt->for_stmt);
        break;
    case STMT_FUNC_DECL:
        create_function(interp, &stmt->func_decl_stmt.def);
        break;
    case STMT_IF:
        if_stmt(interp, &stmt->if_stmt);
        break;
    case STMT_NULL:
        break;
    case STMT_RETURN:
        return_stmt(interp, &stmt->return_stmt);
        return REACHED_RETURN;
    case STMT_VAR_DECL:
        create_variable_expr(interp, stmt->var_decl_stmt->name, stmt->var_decl_stmt->value);
        break;
    case STMT_WHILE:
        while_stmt(interp, &stmt->while_stmt);
        break;
    default:
        invalid_stmt_type(stmt->type);
        break;
    }
    return EXECUTED_STATEMENT;
}

enum run_stmt_result run_current_block(toy_interp *interp)
{
    for (const toy_stmt *s = interp->cur_block->stmts; s; s = s->next) {
        enum run_stmt_result stmt_result;
        stmt_result = run_stmt(interp, s);
        switch (stmt_result) {
        case EXECUTED_STATEMENT:
            break;
        case REACHED_RETURN:
/* TODO: This needs to look up the stack for the most recent func body, not only at the current block */
#if 0
            if (interp->cur_block == NULL) {
                /* In predefined function */
            } else if (interp->cur_block->type == BLOCK_FUNC_BODY) {
                /* In user-defined function */
            } else {
                return_outside_function(interp->cur_block->type);
            }
#endif
            break;
        case REACHED_BREAK:
/* TODO: This needs to look up the stack for the most recent loop, not only at the current block */
#if 0
            if (interp->cur_block == NULL) {
                /* In predefined function */
                assert(0);
            } else if (interp->cur_block->type == BLOCK_LOOP_BODY) {
                return REACHED_BREAK;
            } else {
                break_outside_loop();
            }
#endif
            return stmt_result;
        case REACHED_CONTINUE:
            /* TODO */
            break;
        case REACHED_BLOCK_END:
            assert(0);
            break;
        default:
            assert(0);
            break;
        }
    }
    interp->return_val = null_val;
    return REACHED_BLOCK_END;
}

static enum run_stmt_result run_block(toy_interp *interp, const toy_block *block)
{
    push_context(interp, block);
    enum run_stmt_result block_result = run_current_block(interp);
    switch (block_result) {
    case EXECUTED_STATEMENT:
    case REACHED_CONTINUE:
        assert(0);
        break;
    case REACHED_RETURN:
    case REACHED_BREAK:
    case REACHED_BLOCK_END:
        break;
    default:
        assert(0);
        break;
    }
    pop_context(interp);
    return block_result;
}

toy_interp *alloc_interp(const toy_stmt *program)
{
    toy_interp *interp;
    interp = mymalloc(toy_interp);
    interp->top_block.stmts = (toy_stmt *) program;
    interp->cur_block = &interp->top_block;
    interp->symbols = alloc_map();
    return interp;
}

void free_interp(toy_interp *interp)
{
    free_map(interp->symbols);
    free(interp);
}
