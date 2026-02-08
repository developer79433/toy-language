#ifndef TOY_DECL_REF_LIST_H
#define TOY_DECL_REF_LIST_H 1

#include "decl-ref-types.h"
#include "decl-ref-list-types.h"

decl_ref *decl_ref_list_payload(decl_ref_list *item);
const decl_ref *decl_ref_list_payload_const(const decl_ref_list *item);
void decl_ref_list_dump(const decl_ref_list *list);
void decl_ref_list_assert_valid(const decl_ref_list *list);
decl_ref_list *decl_ref_list_alloc(decl_ref *ref);
decl_ref_list *decl_ref_list_prepend(decl_ref_list *list, decl_ref *ref);
decl_ref *decl_ref_list_find_name(decl_ref_list *list, toy_str desired_name);

#endif /* TOY_DECL_REF_LIST_H */
