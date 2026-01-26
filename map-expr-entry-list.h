#ifndef TOY_MAP_EXPR_ENTRY_LIST_H
#define TOY_MAP_EXPR_ENTRY_LIST_H 1

#include "map-expr-entry-list-types.h"

toy_map_expr_entry_list *map_expr_entry_list_concat(toy_map_expr_entry_list *orig, toy_map_expr_entry_list *new_list);
toy_map_expr_entry_list *map_expr_entry_list_alloc_ref(toy_str first_key, toy_expr *first_value);
toy_map_expr_entry *map_expr_entry_list_payload(toy_map_expr_entry_list *list);
const toy_map_expr_entry *map_expr_entry_list_payload_const(const toy_map_expr_entry_list *list);
void map_expr_entry_list_dump(const toy_map_expr_entry_list *list);

#endif /* TOY_MAP_EXPR_ENTRY_LIST_H */
