#include <stdio.h>

#include "log.h"

void log_debug(const char *str)
{
    fputs(str, stderr);
    fputc('\n', stderr);
}
