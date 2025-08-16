#include <assert.h>

#include "stmt.h"
#include "stmt-list.h"
#include "buf-list.h"
#include "log.h"

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

static item_callback_result stmt_assert_valid_cb(void *cookie, size_t index, const toy_stmt_list *entry)
{
    const toy_stmt *stmt = stmt_list_payload_const(entry);
    stmt_assert_valid(stmt);
    return CONTINUE_ENUMERATION;
}

void stmt_list_assert_valid(const toy_stmt_list *list)
{
    enumeration_result res = stmt_list_foreach_const(list, stmt_assert_valid_cb, NULL);
    assert(res == ENUMERATION_COMPLETE);
}

void stmt_list_dump(const toy_stmt_list *stmts)
{
    /* TODO: Use stmt_list_foreach */
    for (const toy_stmt_list *s = stmts; s; s = s->next) {
        stmt_dump(&s->stmt, 1);
        log_putc('\n');
    }
}
