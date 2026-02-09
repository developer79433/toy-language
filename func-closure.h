#ifndef TOY_FUNC_CLOSURE_H
#define TOY_FUNC_CLOSURE_H 1

#include "func-closure-types.h"
#include "bool-types.h"

void func_closure_assert_valid(const func_closure *closure);
void func_closure_dump(const func_closure *closure, toy_bool verbose);
void func_closure_free(func_closure *closure);

#endif /* TOY_FUNC_CLOSURE_H */
