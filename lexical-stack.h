#ifndef TOY_LEXICAL_STACK_H
#define TOY_LEXICAL_STACK_H 1

#include <stdio.h>

#include "lexical-stack-types.h"
#include "function-types.h"
#include "expr-types.h"

void lexical_stack_init(lexical_stack *stack);
lexical_stack *lexical_stack_push(lexical_stack *stack, lexical_frame *entry);
lexical_stack *lexical_stack_pop(lexical_stack *stack, lexical_frame **removed_entry);
void lexical_stack_dump(const lexical_stack *stack);
toy_function *lexical_stack_cur_function(lexical_stack *stack);
void lexical_stack_resolve(const lexical_stack *stack, toy_str name, resolved_name *resolved);

#endif /* TOY_LEXICAL_STACK_H */
