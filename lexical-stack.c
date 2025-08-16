#include <assert.h>

#include "lexical-stack.h"
#include "buf-stack.h"
#include "log.h"
#include "stmt-list.h"
#include "var-decl-list.h"
#include "str.h"
#include "errors.h"
#include "str-list.h"
#include "function.h"
#include "val.h"
#include "constants.h"
#include "stmt-list.h"
#include "map-size-t.h"
#include "stmt-list.h"
#include "lexical-frame.h"
#include "block.h"
#include "resolved-name.h"

static lexical_frame *lexical_stack_payload(lexical_stack *stack)
{
    return &stack->entry;
}

static const lexical_frame *lexical_stack_payload_const(const lexical_stack *stack)
{
    return &stack->entry;
}

typedef item_callback_result(*lexical_stack_item_callback)(void *cookie, size_t index, lexical_stack *item);
typedef item_callback_result(*const_lexical_stack_item_callback)(void *cookie, size_t index, const lexical_stack *item);

static enumeration_result lexical_stack_foreach(lexical_stack *stack, lexical_stack_item_callback callback, void *cookie)
{
    return buf_stack_foreach((buf_stack *) stack, (buf_stack_item_callback) callback, cookie);
}

static enumeration_result lexical_stack_foreach_const(const lexical_stack *stack, const_lexical_stack_item_callback callback, void *cookie)
{
    return buf_stack_foreach_const((const buf_stack *) stack, (const_buf_stack_item_callback) callback, cookie);
}

void lexical_stack_init(lexical_stack *stack)
{
    buf_stack_init((buf_stack *) stack);
}

typedef struct lexical_stack_entry_dump_cb_args_struct {
    FILE *f;
} lexical_stack_entry_dump_cb_args;

static item_callback_result lexical_stack_entry_dump_cb(void *cookie, size_t index, const lexical_stack *item)
{
    lexical_stack_entry_dump_cb_args *args = (lexical_stack_entry_dump_cb_args *) cookie;
    const lexical_frame *entry = lexical_stack_payload_const(item);
    lexical_frame_dump(args->f, entry);
    return CONTINUE_ENUMERATION;
}

void lexical_stack_dump(FILE *f, const lexical_stack *stack)
{
    log_printf("Lexical stack {\n");
    lexical_stack_entry_dump_cb_args args = { .f = f };
    enumeration_result res = lexical_stack_foreach_const(stack, lexical_stack_entry_dump_cb, &args);
    assert(res == ENUMERATION_COMPLETE);
    log_printf("} End lexical stack\n");
}

lexical_stack *lexical_stack_push(lexical_stack *stack, lexical_frame *entry)
{
    lexical_frame_assert_valid(entry);
    log_debug("Stack before push:\n");
    lexical_stack_dump(stderr, stack);
    lexical_stack *ret = (lexical_stack *) buf_stack_push((buf_stack *) stack, entry, sizeof(*entry));
    log_debug("Stack after push:\n");
    lexical_stack_dump(stderr, ret);
    return ret;
}

lexical_stack *lexical_stack_pop(lexical_stack *stack, lexical_frame **removed_entry)
{
    log_debug("Stack before pop:\n");
    lexical_stack_dump(stderr, stack);
    lexical_stack *ret = (lexical_stack *) buf_stack_pop((buf_stack *) stack, (void **) removed_entry);
    log_debug("Stack after pop:\n");
    lexical_stack_dump(stderr, ret);
    if (removed_entry) {
        lexical_frame *entry = *removed_entry;
        lexical_frame_assert_valid(entry);
    }
    return ret;
}

typedef struct stack_frame_cur_function_cb_args_struct {
    toy_function *found_function;
} stack_frame_cur_function_cb_args;

static item_callback_result stack_frame_cur_function_cb(void *cookie, size_t index, lexical_stack *stack)
{
    stack_frame_cur_function_cb_args *args = (stack_frame_cur_function_cb_args *) cookie;
    lexical_frame *entry = lexical_stack_payload(stack);
    if (entry->type == LEXICAL_FRAME_FUNCTION) {
        lexical_frame_function *func_frame = &entry->function_frame;
        args->found_function = func_frame->function;
        return STOP_ENUMERATION;
    }
    return CONTINUE_ENUMERATION;
}

toy_function *lexical_stack_cur_function(lexical_stack *stack)
{
    stack_frame_cur_function_cb_args args = { .found_function = NULL };
    enumeration_result res = lexical_stack_foreach(stack, stack_frame_cur_function_cb, &args);
    assert(
        ((ENUMERATION_COMPLETE == res) && !args.found_function)
        ||
        ((ENUMERATION_INTERRUPTED == res) && args.found_function)
    );
    return args.found_function;
}

/* TODO: Belongs elsewhere */
const toy_stmt_list *lexical_frame_get_stmts(const lexical_frame *frame)
{
    switch (frame->type) {
    case LEXICAL_FRAME_BLOCK:
        const lexical_frame_function *func_frame = &frame->function_frame;
        return func_frame->function->code.stmts;
        break;
    case LEXICAL_FRAME_FUNCTION:
        const lexical_frame_block *block_frame = &frame->block_frame;
        return block_frame->block->stmts;
        break;
    default:
        assert(0);
        break;
    }
}

typedef struct stack_frame_resolve_cb_args_struct {
    toy_str wanted_name;
    resolved_name *resolved;
} stack_frame_resolve_cb_args;

static item_callback_result stack_frame_resolve_cb(void *cookie, size_t index, const lexical_stack *item)
{
    stack_frame_resolve_cb_args *args = (stack_frame_resolve_cb_args *) cookie;
    const lexical_frame *frame = lexical_stack_payload_const(item);
    lexical_frame_assert_valid(frame);
    lexical_frame_lookup(frame, index, args->wanted_name, args->resolved);
    if (is_resolved(args->resolved)) {
        /* Stop at first (innermost) lexical scope that contains a definition */
        return STOP_ENUMERATION;
    }
    return CONTINUE_ENUMERATION;
}

void lexical_stack_resolve(const lexical_stack *stack, toy_str name, resolved_name *resolved)
{
    assert(stack);
    log_printf("In lexical_stack_resolve\n");
    lexical_stack_dump(stderr, stack);
    stack_frame_resolve_cb_args args = { .wanted_name = name, .resolved = resolved };
    enumeration_result res = lexical_stack_foreach_const(stack, stack_frame_resolve_cb, &args);
    assert(
        ((ENUMERATION_COMPLETE == res) && !is_resolved(args.resolved))
        ||
        ((ENUMERATION_INTERRUPTED == res) && is_resolved(args.resolved))
    );
}
