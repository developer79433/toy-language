#ifndef TOY_RESOLVED_NAME_H
#define TOY_RESOLVED_NAME_H 1

#include "bool-types.h"
#include "expr-types.h"

toy_bool is_resolved(const resolved_name *resolved);
void resolved_name_dump(const resolved_name *resolved);

#endif /* TOY_RESOLVED_NAME_H */
