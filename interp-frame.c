#include "interp-frame.h"
#include "interp.h"
#include "mymalloc.h"
#include "map-val.h"

static const char *frame_type_names[] = {
    "Loop body",
    "If body",
    "Pre-defined function",
    "User-defined function",
    "Block statement"
};

const char *interp_frame_type_name(frame_type type)
{
    return frame_type_names[type];
}

interp_frame *interp_frame_list_payload(interp_frame_list *list)
{
    return &list->frame;
}

const interp_frame *interp_frame_list_payload_const(const interp_frame_list *list)
{
    return &list->frame;
}

interp_frame_list *alloc_frame_loop_body(const toy_block *loop_body, interp_frame_list *prev)
{
    interp_frame_list *frame_list = mymalloc(interp_frame_list);
    frame_list->prev = prev;
    interp_frame *frame = interp_frame_list_payload(frame_list);
    frame->type = FRAME_LOOP_BODY;
    frame->loop_body = loop_body;
    frame->symbols = NULL;
    frame->cur_stmt = loop_body->stmts;
    return frame_list;
}

interp_frame_list *alloc_frame_if_body(const toy_block *if_body, interp_frame_list *prev)
{
    interp_frame_list *frame_list = mymalloc(interp_frame_list);
    frame_list->prev = prev;
    interp_frame *frame = interp_frame_list_payload(frame_list);
    frame->type = FRAME_IF_BODY;
    frame->if_body = if_body;
    frame->symbols = NULL;
    frame->cur_stmt = if_body->stmts;
    return frame_list;
}

interp_frame_list *alloc_frame_block_stmt(const toy_block *block, interp_frame_list *prev)
{
    interp_frame_list *frame_list = mymalloc(interp_frame_list);
    frame_list->prev = prev;
    interp_frame *frame = interp_frame_list_payload(frame_list);
    frame->type = FRAME_BLOCK_STMT;
    frame->block_stmt = block;
    frame->symbols = NULL;
    frame->cur_stmt = block->stmts;
    return frame_list;
}

interp_frame_list *alloc_frame_user_def_func(const toy_function *func_def, interp_frame_list *prev)
{
    interp_frame_list *frame_list = mymalloc(interp_frame_list);
    frame_list->prev = prev;
    interp_frame *frame = interp_frame_list_payload(frame_list);
    frame->type = FRAME_USER_DEF_FUNC;
    frame->user_def_func = func_def;
    frame->symbols = NULL;
    frame->cur_stmt = func_def->code.stmts;
    return frame_list;
}


interp_frame_list *alloc_frame_pre_def_func(const toy_function *func_def, interp_frame_list *prev)
{
    interp_frame_list *frame_list = mymalloc(interp_frame_list);
    frame_list->prev = prev;
    interp_frame *frame = interp_frame_list_payload(frame_list);
    frame->type = FRAME_PRE_DEF_FUNC;
    frame->pre_def_func = func_def;
    frame->symbols = NULL;
    frame->cur_stmt = func_def->code.stmts;
    return frame_list;
}

void interp_frame_free(interp_frame_list *frame_list)
{
    interp_frame *frame = interp_frame_list_payload(frame_list);
    if (frame->symbols) {
        map_val_free(frame->symbols);
    }
    free(frame_list);
}

#if DEBUG_STACK
static void dump_frame(FILE *f, const interp_frame *frame)
{
    switch (frame->type) {
    case FRAME_BLOCK_STMT:
        fprintf(f, "block_stmt %p", frame->block_stmt);
        break;
    case FRAME_IF_BODY:
        fprintf(f, "if_body %p", frame->if_body);
        break;
    case FRAME_LOOP_BODY:
        fprintf(f, "loop_body %p", frame->loop_body);
        break;
    case FRAME_PRE_DEF_FUNC:
        fprintf(f, "Predefined function %s", frame->pre_def_func->name);
        break;
    case FRAME_USER_DEF_FUNC:
        fprintf(f, "User-defined function %s", frame->user_def_func->name);
        break;
    default:
        assert(0);
        break;
    }
    fprintf(stderr, ", symbols=");
    if (frame->symbols) {
        // dump_map_keys(f, frame->symbols);
        dump_map(f, frame->symbols);
    } else {
        fputs("null", f);
    }
}

static void dump_stack(FILE *f, const char *context, const toy_interp *interp)
{
    fprintf(stderr, "STACK %s:\n", context);
    size_t frame_num = 0;
    for (const interp_frame *frame = interp_cur_frame(interp); frame; frame = frame->prev, frame_num++) {
        fprintf(stderr, "  Frame %02lu: ", (unsigned long) frame_num);
        dump_frame(f, frame);
        fprintf(stderr, "\n");
    }
}
#else /* ndef DEBUG_STACK */
#define dump_stack(f, context, interp) do { } while (0)
#endif /* DEBUG_STACK */

void push_context_if_body(toy_interp *interp, const toy_block *block)
{
    interp_frame_list *new_frame = alloc_frame_if_body(block, interp_cur_frame(interp));
    interp_set_cur_frame(interp, new_frame);
    dump_stack(stderr, "after push if body", interp);
}

void push_context_loop_body(toy_interp *interp, const toy_block *block)
{
    interp_frame_list *new_frame = alloc_frame_loop_body(block, interp_cur_frame(interp));
    interp_set_cur_frame(interp, new_frame);
    dump_stack(stderr, "after push loop body", interp);
}

void push_context_pre_def_func(toy_interp *interp, const toy_function *func_def)
{
    interp_frame_list *new_frame = alloc_frame_pre_def_func(func_def, interp_cur_frame(interp));
    interp_set_cur_frame(interp, new_frame);
    dump_stack(stderr, "after push predef func", interp);
}

void push_context_user_def_func(toy_interp *interp, const toy_function *func_def)
{
    interp_frame_list *new_frame = alloc_frame_user_def_func(func_def, interp_cur_frame(interp));
    interp_set_cur_frame(interp, new_frame);
    dump_stack(stderr, "after push user func", interp);
}

void push_context_block_stmt(toy_interp *interp, const toy_block *block)
{
    interp_frame_list *new_frame = alloc_frame_block_stmt(block, interp_cur_frame(interp));
    interp_set_cur_frame(interp, new_frame);
    dump_stack(stderr, "after push block stmt", interp);
}

void pop_context(toy_interp *interp)
{
    interp_frame_list *prev = interp_cur_frame(interp)->prev;
    interp_frame_free(interp_cur_frame(interp));
    interp_set_cur_frame(interp, prev);
    dump_stack(stderr, "after pop", interp);
}

get_result lookup_identifier_in_frame(interp_frame_list *frame_list, toy_val *result, toy_str name)
{
    interp_frame *frame = interp_frame_list_payload(frame_list);
    if (frame->symbols) {
        toy_val *existing_value = map_val_get(frame->symbols, name);
        if (existing_value) {
            *result = *existing_value;
            return GET_FOUND;
        }
    }
    return GET_NOT_FOUND;
}
