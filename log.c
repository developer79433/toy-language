#include <stdio.h>

#include "str.h"
#include "log.h"

static FILE *logfile = NULL;

static void openlog(void)
{
    if (NULL == logfile) {
        logfile = stderr;
    }
}

void log_debug(const char *str)
{
    openlog();
    fputs(str, logfile);
    fputc('\n', logfile);
}

int log_vprintf(const char *fmt, va_list argptr)
{
    openlog();
    return vfprintf(logfile, fmt, argptr);
}

int log_printf(const char *fmt, ...)
{
    va_list argptr;
    va_start(argptr, fmt);
    int retval = log_vprintf(fmt, argptr);
    va_end(argptr);
    return retval;
}

int log_printf_file(const char *filename, const char *fmt, ...)
{
    va_list argptr;
    va_start(argptr, fmt);
    log_printf("%s: ", filename);
    int retval = log_vprintf(fmt, argptr);
    va_end(argptr);
    return retval;
}

int log_putc(int c)
{
    openlog();
    return fputc(c, logfile);
}

int log_puts(const char *str)
{
    openlog();
    return fputs(str, logfile);
}
