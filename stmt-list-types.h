#ifndef TOY_STMT_LIST_TYPES_H
#define TOY_STMT_LIST_TYPES_H 1

#include "stmt-types.h"

struct toy_stmt_list_struct {
    struct toy_stmt_list_struct *next;
    toy_stmt stmt;
};

typedef item_callback_result (*stmt_list_item_callback)(void *cookie, size_t index, toy_stmt_list *item);
typedef item_callback_result (*const_stmt_list_item_callback)(void *cookie, size_t index, const toy_stmt_list *item);

#endif /* TOY_STMT_LIST_TYPES_H */
