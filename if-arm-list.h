#ifndef TOY_IF_ARM_LIST_H
#define TOY_IF_ARM_LIST_H 1

#include "if-arm-list-types.h"
#include "expr-types.h"

void if_arm_list_assert_valid(const toy_if_arm_list *list);
toy_if_arm_list *if_arm_list_alloc(toy_expr *condition, toy_block *code);
toy_if_arm_list *if_arm_list_concat(toy_if_arm_list *list, toy_if_arm_list *new_list);
toy_if_arm *if_arm_list_payload(toy_if_arm_list *list);
const toy_if_arm *if_arm_list_payload_const(const toy_if_arm_list *list);

#endif /* TOY_IF_ARM_LIST_H */
