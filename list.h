#ifndef LIST_H
#define LIST_H 1

#include "ast.h"

size_t list_len(const toy_list *list);
toy_expr *list_index(toy_list *list, toy_num index);
size_t str_list_len(const toy_str_list *list);

#endif /* LIST_H */
