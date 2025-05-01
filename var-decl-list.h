#ifndef TOY_VAR_DECL_LIST_H
#define TOY_VAR_DECL_LIST_H 1

#include <stdio.h>

#include "var-decl-list-types.h"

toy_var_decl_list *var_decl_list_alloc(toy_var_decl *decl);
void var_decl_list_dump(FILE *f, toy_var_decl_list *list);
void var_decl_list_free(toy_var_decl_list *list);
toy_var_decl_list *var_decl_list_concat(toy_var_decl_list *list, toy_var_decl_list *new_list);
toy_var_decl_list *var_decl_list_append(toy_var_decl_list *list, toy_var_decl *new_var_decl);

#endif /* TOY_VAR_DECL_LIST_H */
