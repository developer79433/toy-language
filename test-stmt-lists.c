#include <assert.h>

#include "test-stmt-lists.h"
#include "stmt.h"
#include "stmt-list.h"

void test_stmt_lists(void)
{
    toy_expr expr = { .type = EXPR_LITERAL, .val = { .type = VAL_BOOL, .boolean = TOY_TRUE } };
    toy_stmt stmt = { .type = STMT_EXPR, .expr_stmt.expr = &expr};
    toy_stmt_list *stmt_list = stmt_list_alloc(&stmt);
    assert(stmt_list->next == NULL);
    assert(stmt_list->stmt.type == STMT_EXPR);
    assert(stmt_list->stmt.expr_stmt.expr->type == EXPR_LITERAL);
    assert(stmt_list->stmt.expr_stmt.expr->val.type == VAL_BOOL);
    assert(stmt_list->stmt.expr_stmt.expr->val.boolean == TOY_TRUE);
}
