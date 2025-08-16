#ifndef TOY_BLOCK_H
#define TOY_BLOCK_H 1

#include <stdio.h>

#include "block-types.h"

void block_assert_valid(const toy_block *block);
void block_dump(FILE *f, const toy_block *block);

#endif /* TOY_BLOCK_H */
