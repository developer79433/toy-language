#ifndef TOY_INTERP_FRAME_H
#define TOY_INTERP_FRAME_H 1

#include <stdio.h>

#include "interp-frame-types.h"

const char *interp_frame_type_name(frame_type type);
interp_frame *alloc_frame_loop_body(const toy_block *loop_body, interp_frame *prev);
interp_frame *alloc_frame_if_body(const toy_block *if_body, interp_frame *prev);
void interp_frame_free(interp_frame *frame);
interp_frame *alloc_frame_pre_def_func(const toy_function *func_def, interp_frame *prev);
interp_frame *alloc_frame_user_def_func(const toy_function *func_def, interp_frame *prev);
interp_frame *alloc_frame_block_stmt(const toy_block *block_stmt, interp_frame *prev);
void push_context_pre_def_func(toy_interp *interp, const toy_function *func_def);
void push_context_user_def_func(toy_interp *interp, const toy_function *func_def);
void push_context_block_stmt(toy_interp *interp, const toy_block *block);
void push_context_if_body(toy_interp *interp, const toy_block *block);
void push_context_loop_body(toy_interp *interp, const toy_block *block);
void pop_context(toy_interp *interp);
#if DEBUG_STACK
static void dump_stack(FILE *f, const char *context, const toy_interp *interp);
#else /* ndef DEBUG_STACK */
#define dump_stack(f, context, interp) do { } while (0)
#endif /* DEBUG_STACK */

#endif /* TOY_INTERP_FRAME_H */
