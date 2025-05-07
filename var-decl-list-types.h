#ifndef TOY_VAR_DECL_LIST_TYPES_H
#define TOY_VAR_DECL_LIST_TYPES_H 1

#include "iter-types.h"
#include "var-decl-types.h"

typedef struct toy_var_decl_list_struct {
    struct toy_var_decl_list_struct *next;
    toy_var_decl decl;
} toy_var_decl_list;

typedef item_callback_result (*toy_var_decl_list_item_callback)(void *cookie, size_t index, toy_var_decl_list *item);
typedef item_callback_result (*const_toy_var_decl_list_item_callback)(void *cookie, size_t index, const toy_var_decl_list *item);

#endif /* TOY_VAR_DECL_LIST_TYPES_H */
