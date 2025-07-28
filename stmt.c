#include <assert.h>
#include <string.h>

#include "mymalloc.h"
#include "dump.h"
#include "expr.h"
#include "stmt.h"
#include "str.h"
#include "generic-list.h"
#include "buf-list.h"
#include "var-decl.h"

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
