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

const char *stmt_type_name(enum toy_stmt_type stmt_type)
{
    return toy_stmt_type_names[stmt_type];
}

toy_stmt *stmt_alloc(enum toy_stmt_type type)
{
    toy_stmt *stmt;
    stmt = mymalloc(toy_stmt);
    stmt->type = type;
    return stmt;
}
