#ifndef TOY_LEXICAL_STACK_H
#define TOY_LEXICAL_STACK_H 1

#include "lexical-stack-types.h"
#include "function-types.h"

void lexical_stack_init(lexical_stack *stack);
lexical_stack *lexical_stack_push(lexical_stack *stack, lexical_stack_entry *entry);
lexical_stack *lexical_stack_pop(lexical_stack *stack, lexical_stack_entry **removed_entry);
void lexical_stack_dump(const lexical_stack *stack);
toy_function *cur_function(lexical_stack *stack);
void lexical_stack_resolve(lexical_stack *stack, toy_str name, resolved_name *resolved);
int is_resolved(const resolved_name *resolved);

#endif /* TOY_LEXICAL_STACK_H */
