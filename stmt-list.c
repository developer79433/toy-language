#include <assert.h>

#include "stmt.h"
#include "stmt-list.h"
#include "buf-list.h"
#include "log.h"
#include "list-visitor.h"

toy_stmt_list *stmt_list_alloc(toy_stmt *stmt)
{
    return (toy_stmt_list *) buf_list_alloc(stmt, sizeof(*stmt));
}

void stmt_list_free(toy_stmt_list *stmt_list)
{
    buf_list_free((toy_buf_list *) stmt_list);
}

toy_stmt_list *stmt_list_concat(toy_stmt_list *stmt_list, toy_stmt_list *new_list)
{
    return (toy_stmt_list *) buf_list_concat((toy_buf_list *) stmt_list, (toy_buf_list *) new_list);
}

toy_stmt_list *stmt_list_append(toy_stmt_list *stmt_list, toy_stmt *stmt)
{
    return (toy_stmt_list *) buf_list_append((toy_buf_list *) stmt_list, stmt, sizeof(*stmt));
}

toy_stmt *stmt_list_payload(toy_stmt_list *stmt_list)
{
    return (toy_stmt *) buf_list_payload((toy_buf_list *) stmt_list);
}

const toy_stmt *stmt_list_payload_const(const toy_stmt_list *stmt_list)
{
    return (const toy_stmt *) buf_list_payload_const((const toy_buf_list *) stmt_list);
}

static item_callback_result stmt_assert_valid_cb(const_list_visitor *list_vis, size_t index, const toy_stmt_list *entry)
{
    const toy_stmt *stmt = stmt_list_payload_const(entry);
    stmt_assert_valid(stmt);
    return CONTINUE_ENUMERATION;
}

void stmt_list_assert_valid(const toy_stmt_list *stmt_list)
{
    const_list_visitor list_vis = { .visit_entry = (const_list_entry_visit_func) stmt_assert_valid_cb };
    enumeration_result res = const_list_visitor_visit_list((const_list_visitor *) &list_vis, (const generic_list *) stmt_list);
    assert(res == ENUMERATION_COMPLETE);
}

static item_callback_result stmt_dump_callback(const_list_visitor *list_vis, size_t index, const toy_stmt_list *item)
{
    const toy_stmt *stmt = stmt_list_payload_const(item);
    stmt_dump(stmt, 1);
    log_putc(LOG_DEBUG, '\n');
    return CONTINUE_ENUMERATION;
}

void stmt_list_dump(const toy_stmt_list *stmt_list)
{
    const_list_visitor list_vis = { .visit_entry = (const_list_entry_visit_func) stmt_dump_callback };
    enumeration_result res = const_list_visitor_visit_list((const_list_visitor *) &list_vis, (const generic_list *) stmt_list);
    assert(res == ENUMERATION_COMPLETE);
}

static toy_bool is_desired_stmt(const toy_stmt *desired_stmt, size_t index, const toy_stmt_list *item)
{
    const toy_stmt *stmt = stmt_list_payload_const(item);
    return (stmt == desired_stmt);
}

void stmt_list_assert_contains_stmt(toy_stmt_list *stmt_list, toy_stmt *stmt)
{
    const toy_stmt_list *match = (toy_stmt_list *) list_find_first_const((const generic_list *) stmt_list, (generic_list_filter_func) is_desired_stmt, stmt);
    if (!match) {
        log_error_file("Statement should be in stmt_list. stmt:\n");
        stmt_dump(stmt, TOY_TRUE);
        log_error("\n");
        log_error_file("stmt_list:\n");
        stmt_list_dump(stmt_list);
    }
    assert(match);
    assert(stmt == &match->stmt);
}
