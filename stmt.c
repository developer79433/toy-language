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
