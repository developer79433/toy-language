#ifndef TOY_IF_ARM_LIST_H
#define TOY_IF_ARM_LIST_H 1

#include "if-arm-list-types.h"
#include "expr-types.h"

toy_if_arm_list *if_arm_list_alloc(toy_expr *condition, toy_block *code);
toy_if_arm_list *if_arm_list_concat(toy_if_arm_list *list, toy_if_arm_list *new_list);

#endif /* TOY_IF_ARM_LIST_H */
