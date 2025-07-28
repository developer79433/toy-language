#include <assert.h>

#include "ptr-stack.h"
#include "ptr-list.h"

void *ptr_stack_payload(ptr_stack *stack)
{
    return ptr_list_payload((toy_ptr_list *) stack);
}

const void *ptr_stack_payload_const(const ptr_stack *stack)
{
    return ptr_list_payload_const((const toy_ptr_list *) stack);
}

void ptr_stack_init(ptr_stack *stack)
{
    ptr_list_init((toy_ptr_list *) stack);
}

ptr_stack *ptr_stack_alloc(void *ptr)
{
    return (ptr_stack *) ptr_list_alloc(ptr);
}

size_t ptr_stack_len(const ptr_stack *stack)
{
    return ptr_list_len((const toy_ptr_list *) stack);
}

void ptr_stack_free(ptr_stack *stack)
{
    return ptr_list_free((toy_ptr_list *) stack);
}

ptr_stack *ptr_stack_push(ptr_stack *stack, void *ptr)
{
    return (ptr_stack *) ptr_list_prepend((toy_ptr_list *) stack, ptr);
}

ptr_stack *ptr_stack_pop(ptr_stack *stack, void **removed_ptr)
{
    assert(stack);
    toy_ptr_list *removed_list = NULL;
    toy_ptr_list *ret = ptr_list_remove_first((toy_ptr_list *) stack, &removed_list);
    assert(removed_list);
    if (removed_ptr) {
        *removed_ptr = ptr_list_payload(removed_list);
    }
    ptr_list_free(removed_list);
    return (ptr_stack *) ret;
}

enumeration_result ptr_stack_foreach(ptr_stack *stack, ptr_stack_item_callback callback, void *cookie)
{
    return ptr_list_foreach((toy_ptr_list *) stack, (ptr_list_item_callback) callback, cookie);
}

enumeration_result ptr_stack_foreach_const(const ptr_stack *stack, const_ptr_stack_item_callback callback, void *cookie)
{
    return ptr_list_foreach_const((const toy_ptr_list *) stack, (const_ptr_list_item_callback) callback, cookie);
}
