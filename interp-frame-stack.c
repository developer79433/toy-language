#include <assert.h>

#include "interp-frame-stack.h"
#include "interp-frame.h"
#include "mymalloc.h"
#include "symbol-table.h"
#include "interp.h"
#include "debug.h"
#include "buf-stack.h"
#include "list-visitor.h"
#include "log.h"

typedef struct interp_frame_stack_struct {
    struct interp_frame_stack_struct *next;
    interp_frame frame;
} interp_frame_stack;

interp_frame *interp_frame_stack_payload(interp_frame_stack *stack)
{
    return &stack->frame;
}

const interp_frame *interp_frame_stack_payload_const(const interp_frame_stack *stack)
{
    return &stack->frame;
}

size_t interp_frame_stack_len(const interp_frame_stack *stack)
{
    return buf_stack_len((const buf_stack *) stack);
}

static item_callback_result frame_free_callback(list_visitor *list_vis, size_t index, interp_frame_stack *stack)
{
    interp_frame *frame = interp_frame_stack_payload(stack);
    interp_frame_free(frame);
    return CONTINUE_ENUMERATION;
}

void interp_frame_stack_free(interp_frame_stack *stack)
{
    list_visitor list_vis = { .visit_entry = (list_entry_visit_func) frame_free_callback };
    enumeration_result res = list_visitor_visit_list(&list_vis, (generic_list *) stack);
    assert(ENUMERATION_COMPLETE == res);
}

static item_callback_result frame_assert_valid_callback(list_visitor *list_vis, size_t index, const interp_frame_stack *stack)
{
    const interp_frame *frame = interp_frame_stack_payload_const(stack);
    interp_frame_assert_valid(frame);
    return CONTINUE_ENUMERATION;
}

void interp_frame_stack_assert_valid(const interp_frame_stack *stack)
{
    list_visitor list_vis = { .visit_entry = (list_entry_visit_func) frame_assert_valid_callback };
    enumeration_result res = list_visitor_visit_list(&list_vis, (generic_list *) stack);
    assert(ENUMERATION_COMPLETE == res);
}

static item_callback_result frame_dump_callback(list_visitor *list_vis, size_t index, const interp_frame_stack *item)
{
    const interp_frame *frame = interp_frame_stack_payload_const(item);
    log_debug("  Frame %02zu: ", index);
    interp_frame_dump(frame);
    log_debug("\n");
    return CONTINUE_ENUMERATION;
}

void interp_frame_stack_dump(const char *context, const interp_frame_stack *stack)
{
    fprintf(stderr, "Interpreter stack %s:\n", context);
    const_list_visitor list_vis = { .visit_entry = (const_list_entry_visit_func) frame_dump_callback };
    enumeration_result res = const_list_visitor_visit_list(&list_vis, (const generic_list *) stack);
    assert(res == ENUMERATION_COMPLETE);
}

interp_frame_stack *interp_frame_stack_push(interp_frame_stack *stack, interp_frame *frame)
{
    stack = (interp_frame_stack *) buf_stack_push((buf_stack *) stack, frame, sizeof(*frame));
    return stack;
}

interp_frame_stack *interp_frame_stack_pop(interp_frame_stack *stack, interp_frame **removed_frame)
{
    stack = (interp_frame_stack *) buf_stack_pop((buf_stack *) stack, (void **) removed_frame);
    return stack;
}

interp_frame *interp_frame_stack_index(interp_frame_stack *stack, size_t index)
{
    return (interp_frame *) buf_stack_index((buf_stack *) stack, index);
}

const interp_frame *interp_frame_stack_index_const(const interp_frame_stack *stack, size_t index)
{
    return (const interp_frame *) buf_stack_index_const((const buf_stack *) stack, index);
}
