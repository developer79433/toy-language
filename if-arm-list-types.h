#ifndef TOY_IF_ARM_LIST_TYPES_H
#define TOY_IF_ARM_LIST_TYPES_H 1

#include "if-arm-types.h"

typedef struct toy_if_arm_list_struct {
    struct toy_if_arm_list_struct *next;
    toy_if_arm arm;
} toy_if_arm_list;

#endif /* TOY_IF_ARM_LIST_TYPES_H */
