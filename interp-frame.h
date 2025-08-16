#ifndef TOY_INTERP_FRAME_H
#define TOY_INTERP_FRAME_H 1

#include "interp-frame-types.h"

const char *interp_frame_type_name(frame_type type);
void interp_frame_assert_valid(const interp_frame *frame);
void interp_frame_free(interp_frame *frame);
void interp_frame_dump(const interp_frame *frame);
toy_val *interp_frame_get_var(interp_frame *frame, size_t var_index);
toy_val *interp_frame_get_func_param(interp_frame *frame, size_t param_index);

#endif /* TOY_INTERP_FRAME_H */
