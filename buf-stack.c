#include <assert.h>
#include <stddef.h>

#include "buf-stack.h"
#include "buf-list.h"

void *buf_stack_payload(buf_stack *stack)
{
    return buf_list_payload((toy_buf_list *) stack);
}

const void *buf_stack_payload_const(const buf_stack *stack)
{
    return buf_list_payload_const((const toy_buf_list *) stack);
}

void buf_stack_dump(const buf_stack *stack)
{
    /* TODO */
}

void buf_stack_init(buf_stack *stack)
{
    buf_list_init((toy_buf_list *) stack);
}

buf_stack *buf_stack_alloc(void *buf, size_t size)
{
    return (buf_stack *) buf_list_alloc(buf, size);
}

size_t buf_stack_len(const buf_stack *stack)
{
    return buf_list_len((const toy_buf_list *) stack);
}

void buf_stack_free(buf_stack *stack)
{
    return buf_list_free((toy_buf_list *) stack);
}

buf_stack *buf_stack_push(buf_stack *stack, void *buf, size_t size)
{
    return (buf_stack *) buf_list_prepend((toy_buf_list *) stack, buf, size);
}

buf_stack *buf_stack_pop(buf_stack *stack, void **removed_buf)
{
    assert(stack);
    toy_buf_list *removed_list = NULL;
    toy_buf_list *ret = buf_list_remove_first((toy_buf_list *) stack, &removed_list);
    assert(removed_list);
    if (removed_buf) {
        *removed_buf = buf_list_payload(removed_list);
    }
    return (buf_stack *) ret;
}

void *buf_stack_index(buf_stack *stack, size_t index)
{
    return buf_list_index((toy_buf_list *) stack, index);
}

const void *buf_stack_index_const(const buf_stack *stack, size_t index)
{
    return buf_list_index_const((const toy_buf_list *) stack, index);
}
