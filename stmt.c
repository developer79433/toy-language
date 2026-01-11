#include <assert.h>
#include <string.h>

#include "mymalloc.h"
#include "expr.h"
#include "stmt.h"
#include "stmt-list.h"
#include "str.h"
#include "str-list.h"
#include "generic-list.h"
#include "buf-list.h"
#include "var-decl.h"
#include "var-decl-list.h"
#include "errors.h"
#include "log.h"
#include "function.h"
#include "val.h"
#include "block.h"
#include "toy-parser.h"
#include "if-arm-list.h"

static const char *toy_stmt_type_names[] = {
    "block statement",
    "break statement",
    "continue statement",
    "expression statement",
    "for loop",
    "function declaration",
    "if statement",
    "null statement",
    "return statement",
    "variable declaration",
    "while loop"
};

#ifndef NDEBUG
void assert_toy_stmt_type_valid(toy_stmt_type stmt_type)
{
    assert(stmt_type >= 0);
    assert(stmt_type <= STMT_MAX);
}
#endif /* NDEBUG */

const char *stmt_type_name(toy_stmt_type stmt_type)
{
    assert_toy_stmt_type_valid(stmt_type);
    return toy_stmt_type_names[stmt_type];
}

toy_stmt *stmt_alloc(toy_stmt_type stmt_type)
{
#ifndef NDEBUG
    assert_toy_stmt_type_valid(stmt_type);
#endif /* NDEBUG */
    toy_stmt *stmt;
    stmt = mymalloc(toy_stmt);
    stmt->type = stmt_type;
    return stmt;
}

toy_stmt *func_decl_stmt_alloc(toy_str name, toy_str_list *param_names, toy_block *block)
{
    toy_stmt *stmt = malloc(sizeof(toy_stmt) + sizeof(toy_function) + sizeof(toy_val));
    stmt->type = STMT_FUNC_DECL;
    toy_func_decl_stmt *func_decl = &stmt->func_decl_stmt;
    toy_function *func = func_decl->func = (toy_function *) (stmt + 1);
    func->type = FUNC_USER_DECLARED;
    func->name = name;
    func->code = block;
    func->param_names = param_names;
    func_decl->val = (toy_val *) (func + 1);
    func_decl->val->type = VAL_FUNC;
    func_decl->val->func = func;
    assert(func_decl->val->func == func_decl->func);
    return stmt;
}

void func_decl_stmt_dump(const toy_func_decl_stmt *func_decl)
{
    func_assert_valid(func_decl->func);
    val_assert_valid(func_decl->val);
}

toy_stmt *var_decl_stmt_alloc(toy_var_decl_list *var_decl_list)
{
    toy_stmt *stmt = stmt_alloc(STMT_VAR_DECL);
    stmt->var_decl_stmt.var_decl_list = var_decl_list;
    return stmt;
}

void block_stmt_assert_valid(const toy_block_stmt *block_stmt)
{
    block_assert_valid(block_stmt->block);
}

void expr_stmt_assert_valid(const toy_expr_stmt *expr_stmt)
{
    expr_assert_valid(expr_stmt->expr);
}

void for_stmt_assert_valid(const toy_for_stmt *for_stmt)
{
    stmt_assert_valid(for_stmt->at_end);
    stmt_assert_valid(for_stmt->at_start);
    block_assert_valid(for_stmt->body);
    expr_assert_valid(for_stmt->condition);
}

void func_decl_stmt_assert_valid(const toy_func_decl_stmt *func_decl_stmt)
{
    assert(func_decl_stmt->decl_index >= 0);
    func_assert_valid(func_decl_stmt->func);
    val_assert_valid(func_decl_stmt->val);
}

void if_stmt_assert_valid(const toy_if_stmt *if_stmt)
{
    if_arm_list_assert_valid(if_stmt->arms);
    block_assert_valid(if_stmt->elsepart);
}

void return_stmt_assert_Valid(const toy_return_stmt *return_stmt)
{
    expr_assert_valid(return_stmt->expr);
}

void var_decl_stmt_assert_valid(const toy_var_decl_stmt *var_decl_stmt)
{
    var_decl_list_assert_valid(var_decl_stmt->var_decl_list);
}

void while_stmt_assert_valid(const toy_while_stmt *while_stmt)
{
    block_assert_valid(while_stmt->body);
    expr_assert_valid(while_stmt->condition);
}


void stmt_assert_valid(const toy_stmt *stmt)
{
    switch (stmt->type) {
    case STMT_BLOCK:
        const toy_block_stmt *block_stmt = &stmt->block_stmt;
        block_stmt_assert_valid(block_stmt);
        break;
    case STMT_BREAK:
        break;
    case STMT_CONTINUE:
        break;
    case STMT_EXPR:
        const toy_expr_stmt *expr_stmt = &stmt->expr_stmt;
        expr_stmt_assert_valid(expr_stmt);
        break;
    case STMT_FOR:
        const toy_for_stmt *for_stmt = &stmt->for_stmt;
        for_stmt_assert_valid(for_stmt);
        break;
    case STMT_FUNC_DECL:
        const toy_func_decl_stmt *func_decl_stmt = &stmt->func_decl_stmt;
        func_decl_stmt_assert_valid(func_decl_stmt);
        break;
    case STMT_IF:
        const toy_if_stmt *if_stmt = &stmt->if_stmt;
        if_stmt_assert_valid(if_stmt);
        break;
    case STMT_NULL:
        break;
    case STMT_RETURN:
        const toy_return_stmt *return_stmt = &stmt->return_stmt;
        return_stmt_assert_Valid(return_stmt);
        break;
    case STMT_VAR_DECL:
        const toy_var_decl_stmt *var_decl_stmt = &stmt->var_decl_stmt;
        var_decl_stmt_assert_valid(var_decl_stmt);
        break;
    case STMT_WHILE:
        const toy_while_stmt *while_stmt = &stmt->while_stmt;
        while_stmt_assert_valid(while_stmt);
        break;
    default:
        assert(0);
        break;
    }
}

void stmt_dump(const toy_stmt *stmt, int append_semicolon)
{
    switch (stmt->type) {
    case STMT_BLOCK:
        log_puts("{\n");
        const toy_block_stmt *block_stmt = &stmt->block_stmt;
        stmt_list_dump(block_stmt->block->stmts);
        log_puts("}\n");
        break;
    case STMT_BREAK:
        log_puts("break");
        if (append_semicolon) {
            log_putc(';');
        }
        break;
    case STMT_CONTINUE:
        log_puts("continue");
        if (append_semicolon) {
            log_putc(';');
        }
        break;
    case STMT_EXPR:
        const toy_expr_stmt *expr_stmt = &stmt->expr_stmt;
        expr_dump(expr_stmt->expr);
        if (append_semicolon) {
            log_putc(';');
        }
        break;
    case STMT_FOR:
        log_puts("for (");
        const toy_for_stmt *for_stmt = &stmt->for_stmt;
        if (for_stmt->at_start) {
            stmt_dump(for_stmt->at_start, 1);
        }
        log_putc(' ');
        if (for_stmt->condition) {
            expr_dump(for_stmt->condition);
        } else {
            log_puts("true");
        }
        log_puts("; ");
        if (for_stmt->at_end) {
            stmt_dump(for_stmt->at_end, 0);
        }
        log_puts(") {\n");
        if (for_stmt->body->stmts) {
            stmt_list_dump(for_stmt->body->stmts);
        }
        log_puts("}");
        break;
    case STMT_FUNC_DECL:
        const toy_func_decl_stmt *func_decl_stmt = &stmt->func_decl_stmt;
        const toy_function *func = func_decl_stmt->func;
        log_printf("fun %s(", func->name);
        str_list_dump(func->param_names);
        log_puts(") {\n");
        stmt_list_dump(func->code->stmts);
        log_puts("}");
        break;
    case STMT_IF:
        const toy_if_stmt *if_stmt = &stmt->if_stmt;
        for (toy_if_arm_list *arm_list = if_stmt->arms; arm_list; arm_list = arm_list->next) {
            if (arm_list == if_stmt->arms) {
                log_puts("if (");
            } else {
                log_puts(" elseif (");
            }
            expr_dump(arm_list->arm.condition);
            log_puts(") {\n");
            stmt_list_dump(arm_list->arm.code->stmts);
            log_puts("}");
        }
        if (if_stmt->elsepart->stmts) {
            log_puts(" else {\n");
            stmt_list_dump(if_stmt->elsepart->stmts);
            log_puts("}");
        }
        break;
    case STMT_NULL:
        if (append_semicolon) {
            log_putc(';');
        }
        break;
    case STMT_RETURN:
        log_puts("return ");
        expr_dump(stmt->return_stmt.expr);
        if (append_semicolon) {
            log_putc(';');
        }
        break;
    case STMT_VAR_DECL:
        log_puts("var ");
        var_decl_list_dump(stmt->var_decl_stmt.var_decl_list);
        if (append_semicolon) {
            log_putc(';');
        }
        break;
    case STMT_WHILE:
        const toy_while_stmt *while_stmt = &stmt->while_stmt;
        log_puts("while (\n");
        expr_dump(while_stmt->condition);
        log_puts(") {\n");
        stmt_list_dump(while_stmt->body->stmts);
        /* TODO: block_dump(while_stmt->body.parent); */
        log_puts("}");
        break;
    default:
        invalid_stmt_type(stmt->type);
        break;
    }
}
