#include "stmt.h"
#include "stmt-list.h"
#include "buf-list.h"

toy_stmt_list *stmt_list_alloc(toy_stmt *stmt)
{
    return (toy_stmt_list *) buf_list_alloc(stmt, sizeof(*stmt));
}

toy_stmt_list *stmt_list_concat(toy_stmt_list *list, toy_stmt_list *new_list)
{
    return (toy_stmt_list *) buf_list_concat((toy_buf_list *) list, (toy_buf_list *) new_list);
}

toy_stmt_list *stmt_list_append(toy_stmt_list *list, toy_stmt *stmt)
{
    return (toy_stmt_list *) buf_list_append((toy_buf_list *) list, stmt, sizeof(*stmt));
}
