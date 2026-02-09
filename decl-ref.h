#ifndef TOY_DECL_REF_H
#define TOY_DECL_REF_H 1

#include "decl-ref-types.h"
#include "bool-types.h"
#include "str-types.h"

decl_ref *decl_ref_alloc_predef(const toy_val *val);
const char *decl_ref_type_name(decl_ref_type type);
void decl_ref_dump(const decl_ref *ref);
void decl_ref_assert_valid(const decl_ref *ref);
toy_bool decl_ref_name_matches(const decl_ref *ref, toy_str desired_name);

#endif /* TOY_DECL_REF_H */
