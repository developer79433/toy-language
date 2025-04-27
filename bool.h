#ifndef TOY_BOOL_H
#define TOY_BOOL_H 1

#include <stdio.h>

#include "bool-types.h"

void dump_bool(FILE *f, toy_bool b);
void bool_assert_valid(toy_bool b);

#endif /* TOY_BOOL_H */
