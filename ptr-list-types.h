#ifndef TOY_PTR_LIST_TYPES_H
#define TOY_PTR_LIST_TYPES_H 1

#include "generic-list-types.h"

/* TODO: Rename me to ptr_list */
typedef struct toy_ptr_list_struct {
    struct toy_ptr_list_struct *next;
    void *ptr;
} toy_ptr_list;

#endif /* TOY_PTR_LIST_TYPES_H */
