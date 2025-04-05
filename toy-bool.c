#include <stdio.h>

#include "toy-bool.h"

void dump_bool(FILE *f, toy_bool b)
{
    fputs(b ? "True" : "False", f);
}
