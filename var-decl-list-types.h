#ifndef TOY_VAR_DECL_LIST_TYPES_H
#define TOY_VAR_DECL_LIST_TYPES_H 1

#include "var-decl-types.h"

typedef struct toy_var_decl_list_struct {
    struct toy_var_decl_list_struct *next;
    toy_var_decl decl;
} toy_var_decl_list;

#endif /* TOY_VAR_DECL_LIST_TYPES_H */
