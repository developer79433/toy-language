#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include "mymalloc.h"
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

#if 0
#define DEBUG_STACK 1
#endif

#if 1
#define DEBUG_VARIABLES 1
#endif

typedef struct interp_frame_struct {
    struct interp_frame_struct *prev;
    frame_type type;
    union {
        const toy_block *loop_body;
        const toy_block *if_body;
        const toy_func_def *pre_def_func;
        const toy_func_def *user_def_func;
        const toy_block *block_stmt;
    };
    toy_stmt_list *cur_stmt;
    map_val *symbols;
} interp_frame;

static const char *frame_type_names[] = {
    "Loop body",
    "If body",
    "Pre-defined function",
    "User-defined function",
    "Block statement"
};

const char *frame_type_name(frame_type type)
{
    return frame_type_names[type];
}

typedef struct toy_interp_struct {
    toy_func_def main_program;
    interp_frame *cur_frame;
    toy_val return_val;
} toy_interp;

static run_stmt_result block_stmt(toy_interp *interp, const toy_block *block);

static interp_frame *alloc_frame_loop_body(const toy_block *loop_body, interp_frame *prev)
{
    interp_frame *frame = mymalloc(interp_frame);
    frame->type = FRAME_LOOP_BODY;
    frame->prev = prev;
    frame->loop_body = loop_body;
    frame->symbols = NULL;
    return frame;
}

static interp_frame *alloc_frame_if_body(const toy_block *if_body, interp_frame *prev)
{
    interp_frame *frame = mymalloc(interp_frame);
    frame->type = FRAME_IF_BODY;
    frame->prev = prev;
    frame->if_body = if_body;
    frame->symbols = NULL;
    return frame;
}

static interp_frame *alloc_frame_block_stmt(const toy_block *block_stmt, interp_frame *prev)
{
    interp_frame *frame = mymalloc(interp_frame);
    frame->type = FRAME_BLOCK_STMT;
    frame->prev = prev;
    frame->block_stmt = block_stmt;
    frame->symbols = NULL;
    return frame;
}

static interp_frame *alloc_frame_user_def_func(const toy_func_def *func_def, interp_frame *prev)
{
    interp_frame *frame = mymalloc(interp_frame);
    frame->type = FRAME_USER_DEF_FUNC;
    frame->prev = prev;
    frame->user_def_func = func_def;
    frame->symbols = NULL;
    return frame;
}

static interp_frame *alloc_frame_pre_def_func(const toy_func_def *func_def, interp_frame *prev)
{
    interp_frame *frame = mymalloc(interp_frame);
    frame->type = FRAME_PRE_DEF_FUNC;
    frame->prev = prev;
    frame->pre_def_func = func_def;
    frame->symbols = NULL;
    return frame;
}

static void free_frame(interp_frame *frame)
{
    if (frame->symbols) {
        map_val_free(frame->symbols);
    }
    free(frame);
}

#if DEBUG_STACK
static void dump_frame(FILE *f, const interp_frame *frame)
{
    switch (frame->type) {
    case FRAME_BLOCK_STMT:
        fprintf(f, "block_stmt %p", frame->block_stmt);
        break;
    case FRAME_IF_BODY:
        fprintf(f, "if_body %p", frame->if_body);
        break;
    case FRAME_LOOP_BODY:
        fprintf(f, "loop_body %p", frame->loop_body);
        break;
    case FRAME_PRE_DEF_FUNC:
        fprintf(f, "Predefined function %s", frame->pre_def_func->name);
        break;
    case FRAME_USER_DEF_FUNC:
        fprintf(f, "User-defined function %s", frame->user_def_func->name);
        break;
    default:
        assert(0);
        break;
    }
    fprintf(stderr, ", symbols=");
    if (frame->symbols) {
        // dump_map_keys(f, frame->symbols);
        dump_map(f, frame->symbols);
    } else {
        fputs("null", f);
    }
}

static void dump_stack(FILE *f, const char *context, const toy_interp *interp)
{
    fprintf(stderr, "STACK %s:\n", context);
    size_t frame_num = 0;
    for (const interp_frame *frame = interp->cur_frame; frame; frame = frame->prev, frame_num++) {
        fprintf(stderr, "  Frame %02lu: ", (unsigned long) frame_num);
        dump_frame(f, frame);
        fprintf(stderr, "\n");
    }
}
#else /* ndef DEBUG_STACK */
#define dump_stack(f, context, interp) do { } while (0)
#endif /* DEBUG_STACK */

static void push_context_if_body(toy_interp *interp, const toy_block *block)
{
    interp_frame *new_frame = alloc_frame_if_body(block, interp->cur_frame);
    interp->cur_frame = new_frame;
    interp->cur_frame->cur_stmt = block->stmts;
    dump_stack(stderr, "after push if body", interp);
}

static void push_context_loop_body(toy_interp *interp, const toy_block *block)
{
    interp_frame *new_frame = alloc_frame_loop_body(block, interp->cur_frame);
    interp->cur_frame = new_frame;
    interp->cur_frame->cur_stmt = block->stmts;
    dump_stack(stderr, "after push loop body", interp);
}

static void push_context_pre_def_func(toy_interp *interp, const toy_func_def *func_def)
{
    interp_frame *new_frame = alloc_frame_pre_def_func(func_def, interp->cur_frame);
    interp->cur_frame = new_frame;
    interp->cur_frame->cur_stmt = func_def->code.stmts;
    dump_stack(stderr, "after push predef func", interp);
}

static void push_context_user_def_func(toy_interp *interp, const toy_func_def *func_def)
{
    interp_frame *new_frame = alloc_frame_user_def_func(func_def, interp->cur_frame);
    interp->cur_frame = new_frame;
    interp->cur_frame->cur_stmt = func_def->code.stmts;
    dump_stack(stderr, "after push user func", interp);
}

static void push_context_block_stmt(toy_interp *interp, const toy_block *block)
{
    interp_frame *new_frame = alloc_frame_block_stmt(block, interp->cur_frame);
    interp->cur_frame = new_frame;
    interp->cur_frame->cur_stmt = block->stmts;
    dump_stack(stderr, "after push block stmt", interp);
}

static void pop_context(toy_interp *interp)
{
    interp_frame *prev = interp->cur_frame->prev;
    free_frame(interp->cur_frame);
    interp->cur_frame = prev;
    dump_stack(stderr, "after pop", interp);
}

static int is_predefined(toy_str name)
{
    if (lookup_predefined_constant(name)) {
        return 1;
    }
    if (func_lookup_predef_name(name)) {
        return 1;
    }
    return 0;
}

static int lookup_identifier_in_frame(interp_frame *frame, toy_val *result, toy_str name)
{
    if (frame->symbols) {
        toy_val *existing_value = map_val_get(frame->symbols, name);
        if (existing_value) {
            *result = *existing_value;
            return 1;
        }
    }
    return 0;
}

static int lookup_user_identifier(toy_interp *interp, toy_val *result, toy_str name)
{
    assert(!lookup_predefined_constant(name));
    assert(!func_lookup_predef_name(name));
    for (interp_frame *frame = interp->cur_frame; frame; frame = frame->prev) {
        int found = lookup_identifier_in_frame(frame, result, name);
        if (found) {
            return 1;
        }
    }
    *result = null_val;
    return 0;
}

int lookup_identifier(toy_interp *interp, toy_val *result, const toy_str name)
{
    const toy_val *predef_const = lookup_predefined_constant(name);
    if (predef_const) {
        *result = *((toy_val *) predef_const);
        return 1;
    }
    const toy_func_def *predef_func = func_lookup_predef_name(name);
    if (predef_func) {
        result->type = VAL_FUNC;
        result->func = (toy_func_def *) predef_func;
        return 1;
    }
    return lookup_user_identifier(interp, result, name);
}

enum set_variable_policy_enum {
    POLICY_MUST_ALREADY_EXIST,
    POLICY_MUST_NOT_ALREADY_EXIST
};
typedef enum set_variable_policy_enum set_variable_policy;

static set_result set_symbol(toy_interp *interp, const toy_str name, const toy_val *value)
{
    if (interp->cur_frame->symbols == NULL) {
        interp->cur_frame->symbols = map_val_alloc();
    }
    set_result res = map_val_set(interp->cur_frame->symbols, name, (toy_val *) value);
    return res;
}

/* TODO: Move these into symbol-table.c */
static set_result set_variable_value_policy(toy_interp *interp, const toy_str name, const toy_val *value, set_variable_policy policy)
{
#if DEBUG_VARIABLES
    fprintf(stderr, "Set %s variable '%s' to ", ((POLICY_MUST_ALREADY_EXIST == policy) ? "existing" : "new"), name);
    val_dump(stderr, value);
    fputc('\n', stderr);
#endif /* DEBUG_VARIABLES */
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
    set_result res = set_symbol(interp, name, value);
    assert(
        (res == SET_NEW && policy == POLICY_MUST_NOT_ALREADY_EXIST)
        ||
        (res == SET_EXISTING && policy == POLICY_MUST_ALREADY_EXIST)
    );
    return res;
}

static void set_variable_value(toy_interp *interp, const toy_str name, const toy_val *value)
{
    val_assert_valid(value);
    set_result res = set_variable_value_policy(interp, name, value, POLICY_MUST_ALREADY_EXIST);
    assert(res == SET_EXISTING);
}

static void create_variable_value(toy_interp *interp, const toy_str name, const toy_val *value)
{
    val_assert_valid(value);
    set_result res = set_variable_value_policy(interp, name, value, POLICY_MUST_NOT_ALREADY_EXIST);
    assert(res == SET_NEW);
}

static void create_variable_expr(toy_interp *interp, const toy_str name, const toy_expr *expr)
{
    toy_val value;
    expr_assert_valid(expr);
    eval_expr(interp, &value, expr);
    val_assert_valid(&value);
    create_variable_value(interp, name, &value);
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
    set_result res = set_symbol(interp, def->name, &func_val);
    assert(res == SET_NEW);
}

static void op_assign(toy_interp *interp, toy_val *result, toy_str name, toy_expr *value)
{
    toy_val value_result;
    eval_expr(interp, &value_result, value);
    set_variable_value(interp, name, &value_result);
    *result = value_result;
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
    eval_expr(args->interp, &value, expr);
    assert(args->result->type == VAL_LIST);
    val_list_append(args->result->list, &value);
    return CONTINUE_ENUMERATION;
}

static void eval_expr_list(toy_interp *interp, toy_val *result, const toy_expr_list *expr_list)
{
    result->type = VAL_LIST;
    if (expr_list) {
        toy_val element;
        eval_expr(interp, &element, expr_list->expr);
        result->list = val_list_alloc(&element);
        append_cb_args append_args = { .interp = interp, .result = result };
        /* TODO: Remove this ugly -> next, which is there because we distinguish betwen initial list alloc and append cases */
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

run_stmt_result run_toy_function_expr_list(toy_interp *interp, toy_func_def *def, const toy_expr_list *args)
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

run_stmt_result run_toy_function_val_list(toy_interp *interp, toy_func_def *def, const toy_val_list *args)
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

run_stmt_result run_toy_function_val(toy_interp *interp, toy_func_def *def, const toy_val *arg)
{
    toy_val_list func_args = { .val = *arg, .next = NULL };
    return run_toy_function_val_list(interp, def, &func_args);
}

run_stmt_result call_func(toy_interp *interp, toy_str func_name, const toy_expr_list *args)
{
    run_stmt_result res;
    toy_val expr;
    int already_exists = lookup_identifier(interp, &expr, func_name);
    if (already_exists) {
        if (expr.type == VAL_FUNC) {
            toy_func_def *def = expr.func;
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

static void list_lookup(toy_interp *interp, toy_val *result, toy_val_list *val_list, toy_expr *index)
{
    toy_val index_result;
    eval_expr(interp, &index_result, index);
    if (index_result.type == VAL_NUM) {
        if (index_result.num < 0) {
            invalid_val_list_index(val_list, index_result.num);
        } else {
            toy_val *lookup_result = val_list_index(val_list, index_result.num);
            if (lookup_result) {
                *result = *lookup_result;
            } else {
                invalid_val_list_index(val_list, index_result.num);
            }
        }
    } else {
        invalid_operand(EXPR_COLLECTION_LOOKUP, &index_result);
    }
}

static void map_lookup(toy_interp *interp, toy_val *result, map_val *collection, toy_expr *index)
{
    toy_val index_result;
    eval_expr(interp, &index_result, index);
    if (index_result.type == VAL_STR) {
        toy_val *existing_value = map_val_get(collection, index_result.str);
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

typedef struct map_entry_cb_args_struct {
    toy_interp *interp;
    map_val *map;
} map_entry_cb_args;

static item_callback_result map_entry_callback(void *cookie, size_t index, const toy_map_entry_list *list)
{
    map_entry_cb_args *args = (map_entry_cb_args *) cookie;
    const toy_map_entry *map_entry = map_entry_list_payload_const(list);
    toy_val value;
    eval_expr(args->interp, &value, map_entry->value);
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

void eval_expr(toy_interp *interp, toy_val *result, const toy_expr *expr)
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
        int found = lookup_identifier(interp, result, expr->val.str);
        if (!found) {
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

static toy_bool is_control_result(run_stmt_result stmt_result)
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
static run_stmt_result run_one_stmt(toy_interp *interp, toy_stmt *stmt)
{
    return run_stmt(interp, stmt);
}

static void illegal_instruction_in_for_stmt_at_end(const toy_stmt *stmt)
{
    /* TODO */
    assert(0);
}

static void run_for_stmt_at_end(toy_interp *interp, const toy_for_stmt *for_stmt)
{
    if (for_stmt->at_end) {
        run_stmt_result res = run_one_stmt(interp, for_stmt->at_end);
        if (is_control_result(res)) {
            illegal_instruction_in_for_stmt_at_end(for_stmt->at_end);
        }
    }
}

static void illegal_instruction_in_for_stmt_at_start(const toy_stmt *stmt)
{
    /* TODO */
    assert(0);
}

static void run_for_stmt_at_start(toy_interp *interp, const toy_for_stmt *for_stmt)
{
    if (for_stmt->at_start) {
        run_stmt_result res = run_one_stmt(interp, for_stmt->at_start);
        if (is_control_result(res)) {
            illegal_instruction_in_for_stmt_at_start(for_stmt->at_start);
        }
    }
}

static toy_bool condition_truthy(toy_interp *interp, const toy_expr *expr)
{
    if (expr) {
        toy_val cond_result;
        eval_expr(interp, &cond_result, expr);
        return val_truthy(&cond_result);
    }
    return TOY_TRUE;
}

static toy_bool for_stmt_condition_truthy(toy_interp *interp, const toy_for_stmt *for_stmt)
{
    return condition_truthy(interp, for_stmt->condition);
}

static run_stmt_result for_stmt(toy_interp *interp, const toy_for_stmt *for_stmt)
{
    run_stmt_result res = EXECUTED_STATEMENT;
    for (
        run_for_stmt_at_start(interp, for_stmt);
        /* TRUE */;
        run_for_stmt_at_end(interp, for_stmt)
    ) {
        push_context_loop_body(interp, &for_stmt->body);
        if (!for_stmt_condition_truthy(interp, for_stmt)) {
            pop_context(interp);
            res = EXECUTED_STATEMENT;
            break;
        }
        res = run_current_block(interp);
        pop_context(interp);
        toy_bool break_loop = TOY_FALSE;
        switch (res) {
        case REACHED_RETURN:
            break_loop = TOY_TRUE;
            break;
        case REACHED_BREAK:
            res = EXECUTED_STATEMENT;
            break_loop = TOY_TRUE;
            break;
        case REACHED_CONTINUE:
            continue;
        case REACHED_BLOCK_END:
        case EXECUTED_STATEMENT:
            break;
        default:
            assert(0);
            break;
        }
        if (break_loop) {
            break;
        }
    }
    return res;
}

static run_stmt_result run_if_stmt_block(toy_interp *interp, const toy_block *block)
{
    run_stmt_result res;
    if (block) {
        push_context_if_body(interp, block);
        res = run_current_block(interp);
        pop_context(interp);
    } else {
        res = EXECUTED_STATEMENT;
    }
    return res;
}

static toy_bool if_arm_condition_truthy(toy_interp *interp, const toy_if_arm *arm)
{
        /* TODO: Use if_arm_list_payload */
        return condition_truthy(interp, arm->condition);
}

static run_stmt_result if_stmt(toy_interp *interp, const toy_if_stmt *if_stmt)
{
    const toy_if_arm *found_arm = NULL;
    /* TODO: Use if_arm_list_foreach */
    for (toy_if_arm_list *arm_list = if_stmt->arms; arm_list; arm_list = arm_list->next) {
        if (if_arm_condition_truthy(interp, &arm_list->arm)) {
            found_arm = &arm_list->arm;
            break;
        }
    }
    run_stmt_result res;
    if (found_arm) {
        res = run_if_stmt_block(interp, &found_arm->code);
    } else {
        res = run_if_stmt_block(interp, &if_stmt->elsepart);
    }
    if (res == REACHED_BLOCK_END) {
        res = EXECUTED_STATEMENT;
    }
    return res;
}

static toy_bool while_stmt_condition_truthy(toy_interp *interp, const toy_while_stmt *while_stmt)
{
    return condition_truthy(interp, while_stmt->condition);
}

static run_stmt_result while_stmt(toy_interp *interp, const toy_while_stmt *while_stmt)
{
    run_stmt_result res;
    for (;;) {
        push_context_loop_body(interp, &while_stmt->body);
        if (!while_stmt_condition_truthy(interp, while_stmt)) {
            res = EXECUTED_STATEMENT;
            pop_context(interp);
            break;
        }
        res = run_current_block(interp);
        pop_context(interp);
        toy_bool break_loop = TOY_FALSE;
        switch (res) {
        case REACHED_RETURN:
            break_loop = TOY_TRUE;
            break;
        case REACHED_BREAK:
            res = EXECUTED_STATEMENT;
            break_loop = TOY_TRUE;
            break;
        case REACHED_CONTINUE:
            continue;
        case REACHED_BLOCK_END:
        case EXECUTED_STATEMENT:
            break;
        default:
            assert(0);
            break;
        }
        if (break_loop) {
            break;
        }
    }
    return res;
}

static run_stmt_result return_stmt(toy_interp *interp, const toy_return_stmt *return_stmt)
{
    eval_expr(interp, &interp->return_val, return_stmt->expr);
    return REACHED_RETURN;
}

typedef struct var_decl_cb_args_struct {
    toy_interp *interp;
} var_decl_cb_args;

static item_callback_result var_decl_callback(void *cookie, size_t index, const toy_var_decl_list *var_decl_list)
{
    var_decl_cb_args *args = (var_decl_cb_args *) cookie;
    const toy_var_decl *decl = var_decl_list_payload_const(var_decl_list);
    create_variable_expr(args->interp, decl->name, decl->value);
    return CONTINUE_ENUMERATION;
}

static run_stmt_result var_decl_stmt(toy_interp *interp, const toy_var_decl_list *var_decl_list)
{
    var_decl_cb_args var_decl_args = { .interp = interp };
    enumeration_result res = var_decl_list_foreach_const(var_decl_list, var_decl_callback, &var_decl_args);
    assert(res == ENUMERATION_COMPLETE);
    return EXECUTED_STATEMENT;
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
        eval_expr(interp, &result, stmt->expr_stmt.expr);
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
    for (; interp->cur_frame->cur_stmt; interp->cur_frame->cur_stmt = interp->cur_frame->cur_stmt->next) {
        run_stmt_result stmt_result;
        stmt_result = run_stmt(interp, &interp->cur_frame->cur_stmt->stmt);
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
        interp_frame *prev = interp->cur_frame->prev;
        free_frame(interp->cur_frame);
        interp->cur_frame = prev;
    }
    free(interp);
}
