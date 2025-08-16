#ifndef TOY_STMT_LIST_H
#define TOY_STMT_LIST_H 1

#include "stmt-types.h"
#include "stmt-list-types.h"

void stmt_list_dump(const toy_stmt_list *stmts);
toy_stmt_list *stmt_list_alloc(toy_stmt *stmt);
toy_stmt_list *stmt_list_append(toy_stmt_list *list, toy_stmt *stmt);
toy_stmt_list *stmt_list_concat(toy_stmt_list *list, toy_stmt_list *new_list);
void stmt_list_assert_valid(const toy_stmt_list *list);
toy_stmt *stmt_list_payload(toy_stmt_list *list);
const toy_stmt *stmt_list_payload_const(const toy_stmt_list *list);
enumeration_result stmt_list_foreach(toy_stmt_list *list, stmt_list_item_callback callback, void *cookie);
enumeration_result stmt_list_foreach_const(const toy_stmt_list *list, const_stmt_list_item_callback callback, void *cookie);

#endif /* TOY_STMT_LIST_H */
