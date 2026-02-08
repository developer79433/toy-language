#include <assert.h>
#include <stdio.h>

#include "str.h"
#include "log.h"

static FILE *logfile = NULL;

static const char *log_level_names[LOG_LEVEL_MAX - LOG_LEVEL_MIN + 1] = {
    "Debug",
    "Info",
    "Warning",
    "Error"
};

const char *log_level_name(log_level level)
{
    assert(level >= LOG_LEVEL_MIN && level <= LOG_LEVEL_MAX);
    return log_level_names[level - LOG_LEVEL_MIN];
}

static log_level threshold = LOG_DEBUG;

void log_set_level(log_level new_level)
{
    threshold = new_level;
}

log_level log_get_level(void)
{
    return threshold;
}

static void openlog(void)
{
    if (NULL == logfile) {
        logfile = stderr;
    }
}

/* TODO: Make these static */

void log_putc(log_level level, int c)
{
    if (level >= threshold) {
        openlog();
        fputc(c, logfile);
    }
}

void log_puts(log_level level, const char *str)
{
    if (level >= threshold) {
        openlog();
        fputs(str, logfile);
    }
}

void log_vprintf(log_level level, const char *fmt, va_list argptr)
{
    if (level >= threshold) {
        openlog();
        vfprintf(logfile, fmt, argptr);
    }
}

void log_printf(log_level level, const char *fmt, ...)
{
    va_list argptr;
    va_start(argptr, fmt);
    log_vprintf(level, fmt, argptr);
    va_end(argptr);
}

void log_debug(const char *fmt, ...)
{
    va_list argptr;
    va_start(argptr, fmt);
    log_vprintf(LOG_DEBUG, fmt, argptr);
    va_end(argptr);
}

void log_info(const char *fmt, ...)
{
    va_list argptr;
    va_start(argptr, fmt);
    log_vprintf(LOG_INFO, fmt, argptr);
    va_end(argptr);
}

void log_warn(const char *fmt, ...)
{
    va_list argptr;
    va_start(argptr, fmt);
    log_vprintf(LOG_WARN, fmt, argptr);
    va_end(argptr);
}

void log_error(const char *fmt, ...)
{
    va_list argptr;
    va_start(argptr, fmt);
    log_vprintf(LOG_ERROR, fmt, argptr);
    va_end(argptr);
}

void log_to_file(const char *filename)
{
    openlog();
    FILE *f = fopen(filename, "w");
    if (f) {
        log_info_file("Switching to new log file '%s'\n", filename);
        logfile = f;
        log_info_file("Switched to new log file '%s'\n", filename);
    } else {
        perror("fopen");
        log_warn_file("Failed to open new log file '%s'\n");
    }
}
