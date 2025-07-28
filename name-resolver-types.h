#ifndef TOY_NAME_RESOLVER_TYPES_H
#define TOY_NAME_RESOLVER_TYPES_H 1

#include "lexical-stack-types.h"

struct name_resolver_struct {
    lexical_stack *lexical_scopes;
};
typedef struct name_resolver_struct name_resolver;

#endif /* TOY_NAME_RESOLVER_TYPES_H */
