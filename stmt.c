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
    toy_stmt *stmt = stmt_alloc(STMT_FUNC_DECL);
    stmt->func_decl_stmt.func.type = FUNC_USER_DECLARED;
    stmt->func_decl_stmt.func.parent = NULL;
    stmt->func_decl_stmt.func.name = name;
    stmt->func_decl_stmt.func.param_names = param_names;
    stmt->func_decl_stmt.val.type = VAL_FUNC;
    stmt->func_decl_stmt.val.func = &stmt->func_decl_stmt.func;
    /* FIXME: Structure copy */
    stmt->func_decl_stmt.func.code = *block;
    return stmt;
}

void func_decl_stmt_dump(const toy_func_decl_stmt *func_decl)
{
    /* TODO */
}

toy_stmt *var_decl_stmt_alloc(toy_var_decl_list *var_decl_list)
{
    toy_stmt *stmt = stmt_alloc(STMT_VAR_DECL);
    /* TODO: Eliminate structure assign. Likely memory management bugs here. */
    stmt->var_decl_stmt = *var_decl_list;
    return stmt;
}

void stmt_assert_valid(const toy_stmt *stmt)
{
    /* TODO */
}

void stmt_dump(const toy_stmt *stmt, int append_semicolon)
{
    switch (stmt->type) {
    case STMT_BLOCK:
        log_puts("{\n");
        stmt_list_dump(stmt->block_stmt.block.stmts);
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
        expr_dump(stmt->expr_stmt.expr);
        if (append_semicolon) {
            log_putc(';');
        }
        break;
    case STMT_FOR:
        log_puts("for (");
        if (stmt->for_stmt.at_start) {
            stmt_dump(stmt->for_stmt.at_start, 1);
        }
        log_putc(' ');
        if (stmt->for_stmt.condition) {
            expr_dump(stmt->for_stmt.condition);
        } else {
            log_puts("true");
        }
        log_puts("; ");
        if (stmt->for_stmt.at_end) {
            stmt_dump(stmt->for_stmt.at_end, 0);
        }
        log_puts(") {\n");
        if (stmt->for_stmt.body.stmts) {
            stmt_list_dump(stmt->for_stmt.body.stmts);
        }
        log_puts("}");
        break;
    case STMT_FUNC_DECL:
        log_printf("fun %s(", stmt->func_decl_stmt.func.name);
        str_list_dump(stmt->func_decl_stmt.func.param_names);
        log_puts(") {\n");
        stmt_list_dump(stmt->func_decl_stmt.func.code.stmts);
        log_puts("}");
        break;
    case STMT_IF:
        {
            toy_if_arm_list *arm_list = stmt->if_stmt.arms;
            log_puts("if (");
            expr_dump(arm_list->arm.condition);
            log_puts(") {\n");
            stmt_list_dump(arm_list->arm.code.stmts);
            log_puts("}");
            for (arm_list = arm_list->next; arm_list; arm_list = arm_list->next) {
                log_puts(" elseif (");
                expr_dump(arm_list->arm.condition);
                log_puts(") {\n");
                stmt_list_dump(arm_list->arm.code.stmts);
                log_puts("}");
            }
            if (stmt->if_stmt.elsepart.stmts) {
                log_puts(" else {\n");
                stmt_list_dump(stmt->if_stmt.elsepart.stmts);
                log_puts("}");
            }
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
        var_decl_list_dump(&stmt->var_decl_stmt);
        if (append_semicolon) {
            log_putc(';');
        }
        break;
    case STMT_WHILE:
        log_puts("while (\n");
        expr_dump(stmt->while_stmt.condition);
        log_puts(") {\n");
        stmt_list_dump(stmt->while_stmt.body.stmts);
        log_puts("}");
        break;
    default:
        invalid_stmt_type(stmt->type);
        break;
    }
}
