#ifndef TOY_INTERP_FRAME_H
#define TOY_INTERP_FRAME_H 1

#include <stdio.h>

#include "interp-frame-types.h"

const char *interp_frame_type_name(frame_type type);
get_result interp_frame_lookup_identifier(interp_frame *frame, toy_val *result, toy_str name);
void interp_frame_assert_valid(const interp_frame *frame);
void interp_frame_free(interp_frame *frame);
void interp_frame_dump(FILE *f, const interp_frame *frame);

#endif /* TOY_INTERP_FRAME_H */
