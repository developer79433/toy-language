#ifndef TOY_DECL_REF_LIST_TYPES_H
#define TOY_DECL_REF_LIST_TYPES_H 1

#include "decl-ref-types.h"

struct decl_ref_list_struct;
typedef struct decl_ref_list_struct decl_ref_list;
struct decl_ref_list_struct {
    decl_ref_list *next;
    decl_ref ref;
};

#endif /* TOY_DECL_REF_LIST_TYPES_H */
