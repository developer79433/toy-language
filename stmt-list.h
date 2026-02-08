#ifndef TOY_STMT_LIST_H
#define TOY_STMT_LIST_H 1

#include "stmt-types.h"
#include "stmt-list-types.h"

void stmt_list_dump(const toy_stmt_list *stmt_list);
toy_stmt_list *stmt_list_alloc(toy_stmt *stmt);
void stmt_list_free(toy_stmt_list *stmt_list);
toy_stmt_list *stmt_list_append(toy_stmt_list *stmt_list, toy_stmt *stmt);
toy_stmt_list *stmt_list_concat(toy_stmt_list *stmt_list, toy_stmt_list *new_list);
void stmt_list_assert_valid(const toy_stmt_list *stmt_list);
toy_stmt *stmt_list_payload(toy_stmt_list *stmt_list);
const toy_stmt *stmt_list_payload_const(const toy_stmt_list *stmt_list);
void stmt_list_assert_contains_stmt(toy_stmt_list *stmt_list, toy_stmt *stmt);

#endif /* TOY_STMT_LIST_H */
