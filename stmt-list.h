#ifndef TOY_STMT_LIST_H
#define TOY_STMT_LIST_H 1

#include "stmt-types.h"
#include "stmt-list-types.h"

toy_stmt_list *stmt_list_alloc(toy_stmt *stmt);
toy_stmt_list *stmt_list_append(toy_stmt_list *list, toy_stmt *stmt);
toy_stmt_list *stmt_list_concat(toy_stmt_list *list, toy_stmt_list *new_list);

#endif /* TOY_STMT_LIST_H */
