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

void func_decl_stmt_dump(FILE *f, const toy_func_decl_stmt *func_decl)
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

void stmt_dump(FILE *f, const toy_stmt *stmt, int append_semicolon)
{
    switch (stmt->type) {
    case STMT_BLOCK:
        fputs("{\n", f);
        stmt_list_dump(f, stmt->block_stmt.block.stmts);
        fputs("}\n", f);
        break;
    case STMT_BREAK:
        fputs("break", f);
        if (append_semicolon) {
            fputc(';', f);
        }
        break;
    case STMT_CONTINUE:
        fputs("continue", f);
        if (append_semicolon) {
            fputc(';', f);
        }
        break;
    case STMT_EXPR:
        expr_dump(f, stmt->expr_stmt.expr);
        if (append_semicolon) {
            fputc(';', f);
        }
        break;
    case STMT_FOR:
        fputs("for (", f);
        if (stmt->for_stmt.at_start) {
            stmt_dump(f, stmt->for_stmt.at_start, 1);
        }
        fputc(' ', f);
        if (stmt->for_stmt.condition) {
            expr_dump(f, stmt->for_stmt.condition);
        } else {
            fputs("true", f);
        }
        fputs("; ", f);
        if (stmt->for_stmt.at_end) {
            stmt_dump(f, stmt->for_stmt.at_end, 0);
        }
        fputs(") {\n", f);
        if (stmt->for_stmt.body.stmts) {
            stmt_list_dump(f, stmt->for_stmt.body.stmts);
        }
        fputs("}", f);
        break;
    case STMT_FUNC_DECL:
        fprintf(f, "fun %s(", stmt->func_decl_stmt.func.name);
        str_list_dump(f, stmt->func_decl_stmt.func.param_names);
        fputs(") {\n", f);
        stmt_list_dump(f, stmt->func_decl_stmt.func.code.stmts);
        fputs("}", f);
        break;
    case STMT_IF:
        {
            toy_if_arm_list *arm_list = stmt->if_stmt.arms;
            fputs("if (", f);
            expr_dump(f, arm_list->arm.condition);
            fputs(") {\n", f);
            stmt_list_dump(f, arm_list->arm.code.stmts);
            fputs("}", f);
            for (arm_list = arm_list->next; arm_list; arm_list = arm_list->next) {
                fputs(" elseif (", f);
                expr_dump(f, arm_list->arm.condition);
                fputs(") {\n", f);
                stmt_list_dump(f, arm_list->arm.code.stmts);
                fputs("}", f);
            }
            if (stmt->if_stmt.elsepart.stmts) {
                fputs(" else {\n", f);
                stmt_list_dump(f, stmt->if_stmt.elsepart.stmts);
                fputs("}", f);
            }
        }
        break;
    case STMT_NULL:
        if (append_semicolon) {
            fputc(';', f);
        }
        break;
    case STMT_RETURN:
        fputs("return ", f);
        expr_dump(f, stmt->return_stmt.expr);
        if (append_semicolon) {
            fputc(';', f);
        }
        break;
    case STMT_VAR_DECL:
        fputs("var ", f);
        var_decl_list_dump(f, &stmt->var_decl_stmt);
        if (append_semicolon) {
            fputc(';', f);
        }
        break;
    case STMT_WHILE:
        fputs("while (\n", f);
        expr_dump(f, stmt->while_stmt.condition);
        fputs(") {\n", f);
        stmt_list_dump(f, stmt->while_stmt.body.stmts);
        fputs("}", f);
        break;
    default:
        invalid_stmt_type(stmt->type);
        break;
    }
}
