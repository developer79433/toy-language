#ifndef TOY_LEXICAL_FRAME_H
#define TOY_LEXICAL_FRAME_H 1

#include "lexical-frame-types.h"
#include "expr-types.h"

void lexical_frame_assert_valid(const lexical_frame *entry);
void lexical_frame_func_build_argmap(lexical_frame_function *func_frame);
void lexical_frame_lookup(const lexical_frame *frame, size_t stack_depth, toy_str name, resolved_name *resolved);
const toy_stmt_list *lexical_frame_get_stmts(const lexical_frame *frame);
void lexical_frame_dump(const lexical_frame *entry);

#endif /* TOY_LEXICAL_FRAME_H */
