#ifndef TOY_IF_ARM_LIST_TYPES_H
#define TOY_IF_ARM_LIST_TYPES_H 1

#include "iter-types.h"
#include "if-arm-types.h"

typedef struct toy_if_arm_list_struct {
    struct toy_if_arm_list_struct *next;
    toy_if_arm arm;
} toy_if_arm_list;

typedef item_callback_result (*if_arm_list_item_callback)(void *cookie, size_t index, toy_if_arm_list *item);
typedef item_callback_result (*const_if_arm_list_item_callback)(void *cookie, size_t index, const toy_if_arm_list *item);

#endif /* TOY_IF_ARM_LIST_TYPES_H */
