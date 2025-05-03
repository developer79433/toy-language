#ifndef TOY_IF_ARM_TYPES_H
#define TOY_IF_ARM_TYPES_H 1

#include "stmt-types.h"

struct toy_expr_struct;
typedef struct toy_expr_struct toy_expr;

typedef struct toy_if_arm_struct {
    toy_expr *condition;
    toy_block code;
} toy_if_arm;

#endif /* TOY_IF_ARM_TYPES_H */
