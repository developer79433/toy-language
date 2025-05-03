#ifndef TOY_STMT_H
#define TOY_STMT_H 1

#include <stdio.h>

#include "str-types.h"
#include "expr-types.h"
#include "stmt-types.h"

toy_stmt *stmt_alloc(enum toy_stmt_type type);
const char *stmt_type_name(enum toy_stmt_type stmt_type);

#endif /* TOY_STMT_H */
