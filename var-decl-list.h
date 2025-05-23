#ifndef TOY_VAR_DECL_LIST_H
#define TOY_VAR_DECL_LIST_H 1

#include <stdio.h>

#include "var-decl-list-types.h"
#include "expr.h"

toy_var_decl_list *var_decl_list_alloc(toy_var_decl *decl);
void var_decl_list_dump(FILE *f, const toy_var_decl_list *list);
void var_decl_list_free(toy_var_decl_list *list);
toy_var_decl *var_decl_list_payload(toy_var_decl_list *list);
const toy_var_decl *var_decl_list_payload_const(const toy_var_decl_list *list);
enumeration_result var_decl_list_foreach(toy_var_decl_list *list, toy_var_decl_list_item_callback callback, void *cookie);
enumeration_result var_decl_list_foreach_const(const toy_var_decl_list *list, const_toy_var_decl_list_item_callback callback, void *cookie);
toy_var_decl_list *var_decl_list_concat(toy_var_decl_list *list, toy_var_decl_list *new_list);
toy_var_decl_list *var_decl_list_append(toy_var_decl_list *list, toy_var_decl *new_var_decl);

#endif /* TOY_VAR_DECL_LIST_H */
