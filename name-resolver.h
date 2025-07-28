#ifndef TOY_RESOLVE_REFS_H
#define TOY_RESOLVE_REFS_H 1

#include "function-types.h"
#include "str-types.h"
#include "val-types.h"
#include "expr-types.h"
#include "name-resolver-types.h"

void resolver_init(name_resolver *resolver);
void resolve_names(name_resolver *resolver, toy_function *func);

#endif /* TOY_RESOLVE_REFS_H */
