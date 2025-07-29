#ifndef TOY_INTERP_FRAME_STACK_TYPES_H
#define TOY_INTERP_FRAME_STACK_TYPES_H 1

#include "interp-frame-types.h"
#include "buf-stack-types.h"

typedef struct interp_frame_stack_struct interp_frame_stack;

typedef item_callback_result (*interp_frame_stack_item_callback)(void *cookie, size_t index, interp_frame_stack *item);
typedef item_callback_result (*const_interp_frame_stack_item_callback)(void *cookie, size_t index, const interp_frame_stack *item);

#endif /* TOY_INTERP_FRAME_STACK_TYPES_H */
