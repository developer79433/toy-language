#ifndef TOY_INTERP_FRAME_STACK_TYPES_H
#define TOY_INTERP_FRAME_STACK_TYPES_H 1

#include "interp-frame-types.h"
#include "buf-stack-types.h"

typedef struct interp_frame_stack_struct {
    struct interp_frame_stack_struct *prev;
    interp_frame frame;
} interp_frame_stack;

#endif /* TOY_INTERP_FRAME_STACK_TYPES_H */
