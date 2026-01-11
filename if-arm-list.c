#include <assert.h>

#include "mymalloc.h"
#include "if-arm-list.h"
#include "expr.h"
#include "buf-list.h"
#include "block.h"

toy_if_arm_list *if_arm_list_alloc(toy_expr *condition, toy_block *block)
{
    toy_if_arm_list *arm_list;
    arm_list = mymalloc(toy_if_arm_list);
    arm_list->arm.condition = condition;
    arm_list->arm.code = block;
    arm_list->next = NULL;
    return arm_list;
}

toy_if_arm_list *if_arm_list_concat(toy_if_arm_list *list, toy_if_arm_list *new_list)
{
    return (toy_if_arm_list *) buf_list_concat((toy_buf_list *) list, (toy_buf_list *) new_list);
}

toy_if_arm *if_arm_list_payload(toy_if_arm_list *list)
{
    return &list->arm;
}

const toy_if_arm *if_arm_list_payload_const(const toy_if_arm_list *list)
{
    return &list->arm;
}

enumeration_result if_arm_list_foreach(toy_if_arm_list *list, if_arm_list_item_callback callback, void *cookie)
{
    return buf_list_foreach((toy_buf_list *) list, (buf_list_item_callback) callback, cookie);
}

enumeration_result if_arm_list_foreach_const(const toy_if_arm_list *list, const_if_arm_list_item_callback callback, void *cookie)
{
    return buf_list_foreach_const((const toy_buf_list *) list, (const_buf_list_item_callback) callback, cookie);
}

/* TODO: Belongs elsewhere */
static void if_arm_assert_valid(const toy_if_arm *if_arm)
{
    block_assert_valid(if_arm->code);
    expr_assert_valid(if_arm->condition);
}

static item_callback_result arm_valid_cb(void *cookie, size_t index, const toy_if_arm_list *item)
{
    const toy_if_arm *arm = if_arm_list_payload_const(item);
    if_arm_assert_valid(arm);
    return CONTINUE_ENUMERATION;
}

void if_arm_list_assert_valid(const toy_if_arm_list *list)
{
    enumeration_result res = if_arm_list_foreach_const(list, arm_valid_cb, NULL);
    assert(ENUMERATION_COMPLETE == res);
}
