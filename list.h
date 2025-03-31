#ifndef LIST_H
#define LIST_H 1

#include "ast.h"

size_t list_len(const toy_list *list);
toy_expr *list_index(toy_list *list, toy_num index);

#endif /* LIST_H */
