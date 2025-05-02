#include <stdio.h>

#include "log.h"

void log_debug(const char *str)
{
    fputs(str, stderr);
    fputc('\n', stderr);
}

int log_vprintf(const char *fmt, va_list argptr)
{
    return vfprintf(stderr, fmt, argptr);
}

int log_printf(const char *fmt, ...)
{
    va_list argptr;
    va_start(argptr, fmt);
    int retval = log_vprintf(fmt, argptr);
    va_end(argptr);
    return retval;
}
