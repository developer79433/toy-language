#include <assert.h>
#include <string.h>

#include "str.h"
#include "errors.h"
#include "stmt.h"
#include "str-list.h"
#include "stmt-list.h"
#include "name-resolver.h"
#include "var-decl-list.h"
#include "constants.h"
#include "function.h"
#include "predef-function.h"
#include "expr-list.h"
#include "if-arm-list.h"
#include "name-resolver-types.h"
#include "map-val.h"
#include "val-list.h"
#include "log.h"
#include "lexical-stack.h"
#include "constants.h"
#include "predef-function.h"
#include "map-size-t.h"
#include "resolved-name.h"

void resolver_init(name_resolver *resolver)
{
    resolver->lexical_scopes = NULL;
}

static void resolve_names_expr(name_resolver *resolver, toy_expr *expr);

static void resolve_name(name_resolver *resolver, toy_str name, resolved_name *resolved)
{
    assert(resolver);
    assert(name);
    assert(strlen(name));
    log_printf("Resolving name '%s'\n", name);
    resolved->type = REF_UNDEFINED;
    lexical_stack_resolve(resolver->lexical_scopes, name, resolved);
    if (is_resolved(resolved)) {
        resolved_name_dump(resolved);
    } else {
        log_printf("Resolving predef constant '%s'\n", name);
        const predefined_constant *predef_const = lookup_predefined_constant(name);
        if (predef_const) {
            resolved->type = REF_PREDEF_CONST;
            resolved->predef_const = predef_const;
        } else {
            log_printf("Resolving predef func '%s'\n", name);
            const toy_val *val = predef_func_lookup_name(name);
            if (val) {
                assert(VAL_FUNC == val->type);
                const toy_function *func = val->func;
                assert(FUNC_PREDEFINED == func->type);
                resolved->type = REF_PREDEF_FUNC;
                resolved->predef_func = val;
            } else {
                undeclared_identifier(name);
            }
        }
    }
}

static void resolve_names_unary_op(name_resolver *resolver, toy_unary_op *unop)
{
    resolve_names_expr(resolver, unop->arg);
}

static void resolve_names_binop(name_resolver *resolver, toy_binary_op *binop)
{
    resolve_names_expr(resolver, binop->arg1);
    resolve_names_expr(resolver, binop->arg2);
}

static void resolve_names_postfix_decrement(name_resolver *resolver, toy_postfix_decrement *postdec)
{
    resolve_name(resolver, postdec->id, &postdec->resolved);
}

static void resolve_names_postfix_increment(name_resolver *resolver, toy_postfix_increment *postinc)
{
    resolve_name(resolver, postinc->id, &postinc->resolved);
}

static void resolve_names_prefix_decrement(name_resolver *resolver, toy_prefix_decrement *predec)
{
    resolve_name(resolver, predec->id, &predec->resolved);
}

static void resolve_names_prefix_increment(name_resolver *resolver, toy_prefix_increment *preinc)
{
    resolve_name(resolver, preinc->id, &preinc->resolved);
}

static void resolve_names_ternary(name_resolver *resolver, toy_ternary *tern)
{
    resolve_names_expr(resolver, tern->condition);
    resolve_names_expr(resolver, tern->if_true);
    resolve_names_expr(resolver, tern->if_false);
}

typedef struct expr_cb_args_struct {
    name_resolver *resolver;
} expr_cb_args;

static item_callback_result resolve_expr_list_entry_callback(void *cookie, size_t index, toy_expr_list *item)
{
    expr_cb_args *args = (expr_cb_args *) cookie;
    toy_expr *expr = expr_list_payload(item);
    resolve_names_expr(args->resolver, expr);
    return CONTINUE_ENUMERATION;
}

static void resolve_names_expr_list(name_resolver *resolver, toy_expr_list *expr)
{
    expr_cb_args args = { .resolver = resolver };
    enumeration_result res = expr_list_foreach(expr, resolve_expr_list_entry_callback, &args);
    assert(res == ENUMERATION_COMPLETE);
}

static void resolve_names_val(name_resolver *resolver, toy_val *val);

typedef struct val_list_entry_cb_args_struct {
    name_resolver *resolver;
} val_list_entry_cb_args;

static item_callback_result resolve_val_list_entry_callback(void *cookie, size_t index, toy_val_list *entry)
{
    val_list_entry_cb_args *args = (val_list_entry_cb_args *) cookie;
    toy_val *val = val_list_payload(entry);
    resolve_names_val(args->resolver, val);
    return CONTINUE_ENUMERATION;
}

static void resolve_names_val_list(name_resolver *resolver, toy_val_list *val_list)
{
    val_list_entry_cb_args args = { .resolver = resolver };
    enumeration_result res = val_list_foreach(val_list, resolve_val_list_entry_callback, &args);
    assert(res == ENUMERATION_COMPLETE);
}

typedef struct map_val_entry_cb_args_struct {
    name_resolver *resolver;
} map_val_entry_cb_args;

static item_callback_result resolve_map_val_entry_callback(void *cookie, map_val_entry *entry)
{
    map_val_entry_cb_args *args = (map_val_entry_cb_args *) cookie;
    resolve_names_val(args->resolver, &entry->value);
    return CONTINUE_ENUMERATION;
}

static void resolve_names_map_val(name_resolver *resolver, map_val *val_map)
{
    map_val_entry_cb_args args = { .resolver = resolver };
    enumeration_result res = map_val_foreach(val_map, resolve_map_val_entry_callback, &args);
    assert(ENUMERATION_COMPLETE == res);
}

static void resolve_names_func(name_resolver *resolver, toy_function *func);

static void resolve_names_val(name_resolver *resolver, toy_val *val)
{
    switch (val->type) {
    case VAL_BOOL:
        break;
    case VAL_FUNC:
        resolve_names_func(resolver, val->func);
        break;
    case VAL_LIST:
        resolve_names_val_list(resolver, val->list);
        break;
    case VAL_MAP:
        resolve_names_map_val(resolver, val->map);
        break;
    case VAL_NULL:
    case VAL_NUM:
    case VAL_STR:
        break;
    default:
        assert(0);
        break;
    }
}

typedef struct map_entry_cb_args_struct {
    name_resolver *resolver;
} map_entry_cb_args;

static item_callback_result resolve_map_entry_callback(void *cookie, size_t index, toy_map_entry_list *item)
{
    map_entry_cb_args *args = (map_entry_cb_args *) cookie;
    toy_map_entry *entry = map_entry_list_payload(item);
    resolve_names_expr(args->resolver, entry->value);
    return CONTINUE_ENUMERATION;
}

void resolve_names_map_entry_list(name_resolver *resolver, toy_map_entry_list *entry_list)
{
    map_entry_cb_args args = { .resolver = resolver };
    enumeration_result res = map_entry_list_foreach(entry_list, resolve_map_entry_callback, &args);
    assert(res == ENUMERATION_COMPLETE);
}

static void resolve_names_expr(name_resolver *resolver, toy_expr *expr)
{
    switch (expr->type) {
    case EXPR_AND:
        resolve_names_binop(resolver, &expr->binary_op);
        break;
    case EXPR_ASSIGN:
        toy_assignment *assignment = &expr->assignment;
        resolve_name(resolver, assignment->lhs, &assignment->resolved);
        resolve_names_expr(resolver, assignment->rhs);
        break;
    case EXPR_COLLECTION_LOOKUP:
        toy_collection_lookup *lookup = &expr->collection_lookup;
        resolve_name(resolver, lookup->lhs, &lookup->resolved);
        resolve_names_expr(resolver, lookup->rhs);
        break;
    case EXPR_COMMA:
    case EXPR_DIV:
    case EXPR_EQUAL:
    case EXPR_EXPONENT:
        resolve_names_binop(resolver, &expr->binary_op);
        break;
    case EXPR_FIELD_REF:
        toy_field_ref *field_ref = &expr->field_ref;
        resolve_name(resolver, field_ref->lhs, &field_ref->resolved);
        break;
    case EXPR_FUNC_CALL:
        toy_func_call *call = &expr->func_call;
        resolve_name(resolver, call->id, &call->resolved);
        resolve_names_expr_list(resolver, call->args);
        break;
    case EXPR_GT:
    case EXPR_GTE:
        resolve_names_binop(resolver, &expr->binary_op);
        break;
    case EXPR_IDENTIFIER:
        toy_id_expr *id_expr = &expr->id;
        resolve_name(resolver, id_expr->id, &id_expr->resolved);
        break;
    case EXPR_IN:
        resolve_names_binop(resolver, &expr->binary_op);
        break;
    case EXPR_LIST:
        resolve_names_expr_list(resolver, expr->list);
        break;
    case EXPR_LITERAL:
        toy_val *val = &expr->val;
        resolve_names_val(resolver, val);
        break;
    case EXPR_LT:
    case EXPR_LTE:
        resolve_names_binop(resolver, &expr->binary_op);
        break;
    case EXPR_MAP:
        toy_map_entry_list *entry_list = expr->map;
        resolve_names_map_entry_list(resolver, entry_list);
        break;
    case EXPR_METHOD_CALL:
        toy_method_call *method_call = &expr->method_call;
        resolve_name(resolver, method_call->lhs, &method_call->resolved_lhs);
        resolve_name(resolver, method_call->method_name, &method_call->resolved_method);
        break;
    case EXPR_MINUS:
    case EXPR_MODULUS:
    case EXPR_MUL:
    case EXPR_NEQUAL:
        resolve_names_binop(resolver, &expr->binary_op);
        break;
    case EXPR_NOT:
        resolve_names_unary_op(resolver, &expr->unary_op);
        break;
    case EXPR_PLUS:
        resolve_names_binop(resolver, &expr->binary_op);
        break;
    case EXPR_POSTFIX_DECREMENT:
        resolve_names_postfix_decrement(resolver, &expr->postfix_decrement);
        break;
    case EXPR_POSTFIX_INCREMENT:
        resolve_names_postfix_increment(resolver, &expr->postfix_increment);
        break;
    case EXPR_PREFIX_DECREMENT:
        resolve_names_prefix_decrement(resolver, &expr->prefix_decrement);
        break;
    case EXPR_PREFIX_INCREMENT:
        resolve_names_prefix_increment(resolver, &expr->prefix_increment);
        break;
    case EXPR_TERNARY:
        resolve_names_ternary(resolver, &expr->ternary);
        break;
    case EXPR_UNEG:
        resolve_names_unary_op(resolver, &expr->unary_op);
        break;
    default:
        assert(0);
        break;
    }
}

static void resolve_names_stmt(name_resolver *resolver, toy_stmt *stmt);

typedef struct stmt_resolve_cb_args_struct {
    name_resolver *resolver;
} stmt_resolve_cb_args;

static item_callback_result resolve_stmt_callback(void *cookie, size_t index, toy_stmt_list *item)
{
    stmt_resolve_cb_args *args = (stmt_resolve_cb_args *) cookie;
    toy_stmt *stmt = stmt_list_payload(item);
    resolve_names_stmt(args->resolver, stmt);
    return CONTINUE_ENUMERATION;
}

static void resolve_names_stmt_list(name_resolver *resolver, toy_stmt_list *stmt_list)
{
    log_debug("In resolve_names_stmt_list\n");
    stmt_resolve_cb_args args = { .resolver = resolver };
    enumeration_result res = stmt_list_foreach(stmt_list, resolve_stmt_callback, &args);
    assert(ENUMERATION_COMPLETE == res);
}

static void resolve_names_block(name_resolver *resolver, toy_block *block)
{
    lexical_frame stack_entry = { .type = LEXICAL_FRAME_BLOCK, .block_frame = { .block = block } };
    map_size_t_init(&stack_entry.variables);
    resolver->lexical_scopes = lexical_stack_push(resolver->lexical_scopes, &stack_entry);
    resolve_names_stmt_list(resolver, block->stmts);
    resolver->lexical_scopes = lexical_stack_pop(resolver->lexical_scopes, NULL);
}

static void resolve_names_func(name_resolver *resolver, toy_function *func)
{
    log_debug("In resolve_names_func\n");
    lexical_frame stack_entry = { .type = LEXICAL_FRAME_FUNCTION, .function_frame = { .function = func } };
    map_size_t_init(&stack_entry.variables);
    map_size_t_init(&stack_entry.function_frame.arguments);
    resolver->lexical_scopes = lexical_stack_push(resolver->lexical_scopes, &stack_entry);
    resolve_names_stmt_list(resolver, func->code.stmts);
    resolver->lexical_scopes = lexical_stack_pop(resolver->lexical_scopes, NULL);
}

typedef struct if_arm_cb_args_struct {
    name_resolver *resolver;
} if_arm_cb_args;

static item_callback_result resolve_if_arm_callback(void *cookie, size_t index, toy_if_arm_list *entry)
{
    if_arm_cb_args *args = (if_arm_cb_args *) cookie;
    toy_if_arm *arm = if_arm_list_payload(entry);
    resolve_names_expr(args->resolver, arm->condition);
    resolve_names_block(args->resolver, &arm->code);
    return CONTINUE_ENUMERATION;
}

static void resolve_names_if_arm_list(name_resolver *resolver, toy_if_arm_list *if_arm_list)
{
    if_arm_cb_args args = { .resolver = resolver };
    enumeration_result res = if_arm_list_foreach(if_arm_list, resolve_if_arm_callback, &args);
    assert(ENUMERATION_COMPLETE == res);
}

typedef struct var_decl_cb_args_struct {
    name_resolver *resolver;
} var_decl_cb_args;

static item_callback_result resolve_var_decl_callback(void *cookie, size_t index, toy_var_decl_list *entry)
{
    var_decl_cb_args *args = (var_decl_cb_args *) cookie;
    toy_var_decl *decl = var_decl_list_payload(entry);
    resolve_names_expr(args->resolver, decl->value);
    return CONTINUE_ENUMERATION;
}

static void resolve_names_var_decl_list(name_resolver *resolver, toy_var_decl_list *var_decl_list)
{
    var_decl_cb_args args = { .resolver = resolver };
    enumeration_result res = var_decl_list_foreach(var_decl_list, resolve_var_decl_callback, &args);
    assert(ENUMERATION_COMPLETE == res);
}

static void resolve_names_stmt(name_resolver *resolver, toy_stmt *stmt)
{
    switch (stmt->type) {
    case STMT_BLOCK:
        toy_block *block = &stmt->block_stmt.block;
        resolve_names_block(resolver, block);
        break;
    case STMT_BREAK:
    case STMT_CONTINUE:
        break;
    case STMT_EXPR:
        toy_expr *expr = stmt->expr_stmt.expr;
        resolve_names_expr(resolver, expr);
        break;
    case STMT_FOR:
        toy_for_stmt *for_stmt = &stmt->for_stmt;
        resolve_names_stmt(resolver, for_stmt->at_end);
        resolve_names_stmt(resolver, for_stmt->at_start);
        resolve_names_block(resolver, &for_stmt->body);
        resolve_names_expr(resolver, for_stmt->condition);
        break;
    case STMT_FUNC_DECL:
        toy_func_decl_stmt *fdecl = &stmt->func_decl_stmt;
        resolve_names_func(resolver, &fdecl->func);
        break;
    case STMT_IF:
        toy_if_stmt *if_stmt = &stmt->if_stmt;
        resolve_names_if_arm_list(resolver, if_stmt->arms);
        resolve_names_block(resolver, &if_stmt->elsepart);
        break;
    case STMT_NULL:
        break;
    case STMT_RETURN:
        toy_return_stmt *return_stmt = &stmt->return_stmt;
        resolve_names_expr(resolver, return_stmt->expr);
        break;
    case STMT_VAR_DECL:
        toy_var_decl_list *vdecl_list = &stmt->var_decl_stmt;
        resolve_names_var_decl_list(resolver, vdecl_list);
        break;
    case STMT_WHILE:
        toy_while_stmt *while_stmt = &stmt->while_stmt;
        resolve_names_expr(resolver, while_stmt->condition);
        resolve_names_block(resolver, &while_stmt->body);
        break;
    default:
        assert(0);
        break;
    }
}

void resolve_names(name_resolver *resolver, toy_function *func)
{
    assert(func->type == FUNC_USER_DECLARED);
    resolve_names_func(resolver, func);
}
