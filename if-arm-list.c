#include "mymalloc.h"
#include "if-arm-list.h"
#include "expr.h"
#include "buf-list.h"

toy_if_arm_list *if_arm_list_alloc(toy_expr *condition, toy_block *block)
{
    toy_if_arm_list *arm_list;
    arm_list = mymalloc(toy_if_arm_list);
    arm_list->arm.condition = condition;
    arm_list->arm.code.stmts = block->stmts;
    arm_list->next = NULL;
    return arm_list;
}

toy_if_arm_list *if_arm_list_concat(toy_if_arm_list *list, toy_if_arm_list *new_list)
{
    return (toy_if_arm_list *) buf_list_concat((toy_buf_list *) list, (toy_buf_list *) new_list);
}
