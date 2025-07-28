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

toy_stmt *stmt_list_payload(toy_stmt_list *list)
{
    return (toy_stmt *) buf_list_payload((toy_buf_list *) list);
}

const toy_stmt *stmt_list_payload_const(const toy_stmt_list *list)
{
    return (const toy_stmt *) buf_list_payload_const((const toy_buf_list *) list);
}

enumeration_result stmt_list_foreach(toy_stmt_list *list, stmt_list_item_callback callback, void *cookie)
{
    return buf_list_foreach((toy_buf_list *) list, (buf_list_item_callback) callback, cookie);
}

enumeration_result stmt_list_foreach_const(const toy_stmt_list *list, const_stmt_list_item_callback callback, void *cookie)
{
    return buf_list_foreach_const((const toy_buf_list *) list, (const_buf_list_item_callback) callback, cookie);
}
