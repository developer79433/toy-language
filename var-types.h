#ifndef TOY_VAR_TYPES_H
#define TOY_VAR_TYPES_H 1

#include <stddef.h>

#include "val-types.h"

typedef struct toy_var_struct {
    toy_val *val;
    size_t num_refs;
} toy_var;

#endif /* TOY_VAR_TYPES_H */
