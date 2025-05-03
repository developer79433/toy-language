#ifndef TOY_STMT_LIST_TYPES_H
#define TOY_STMT_LIST_TYPES_H 1

#include "stmt-types.h"

struct toy_stmt_list_struct {
    struct toy_stmt_list_struct *next;
    toy_stmt stmt;
};

#endif /* TOY_STMT_LIST_TYPES_H */
