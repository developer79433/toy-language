#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <wchar.h>

#include "str.h"
#include "log.h"
#include "fwopen.h"

static FILE *logfile = NULL;

static const toy_str log_level_names[LOG_LEVEL_MAX - LOG_LEVEL_MIN + 1] = {
    L"Debug",
    L"Info",
    L"Warning",
    L"Error"
};

const toy_str log_level_name(log_level level)
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

void log_putc(log_level level, toy_char c)
{
    if (level >= threshold) {
        openlog();
        fputwc(c, logfile);
    }
}

void log_puts(log_level level, const toy_str str)
{
    if (level >= threshold) {
        openlog();
        fputws(str, logfile);
    }
}

void log_vprintf(log_level level, const toy_str fmt, va_list argptr)
{
    if (level >= threshold) {
        openlog();
        vfwprintf(logfile, fmt, argptr);
    }
}

void log_printf(log_level level, const toy_str fmt, ...)
{
    va_list argptr;
    va_start(argptr, fmt);
    log_vprintf(level, fmt, argptr);
    va_end(argptr);
}

void log_debug(const toy_str fmt, ...)
{
    va_list argptr;
    va_start(argptr, fmt);
    log_vprintf(LOG_DEBUG, fmt, argptr);
    va_end(argptr);
}

void log_info(const toy_str fmt, ...)
{
    va_list argptr;
    va_start(argptr, fmt);
    log_vprintf(LOG_INFO, fmt, argptr);
    va_end(argptr);
}

void log_warn(const toy_str fmt, ...)
{
    va_list argptr;
    va_start(argptr, fmt);
    log_vprintf(LOG_WARN, fmt, argptr);
    va_end(argptr);
}

void log_error(const toy_str fmt, ...)
{
    va_list argptr;
    va_start(argptr, fmt);
    log_vprintf(LOG_ERROR, fmt, argptr);
    va_end(argptr);
}

void log_to_file(const toy_str filename)
{
    openlog();
    FILE *f = fwopen(filename, "w");
    if (f) {
        log_info_file(L"Switching to new log file '%ls'\n", filename);
        logfile = f;
        log_info_file(L"Switched to new log file '%ls'\n", filename);
    } else {
        perror("fopen");
        log_warn_file(L"Failed to open new log file '%ls'\n");
    }
}

void hex_dump(const void *ptr, size_t len)
{
    for (const uint8_t *p = (const uint8_t *) ptr; p < ((const uint8_t *) ptr) + len; p++) {
        if (isprint(*p)) {
            log_debug(L"%lc", *p);
        } else {
            log_debug(L"\\x%02x", *p);
        }
    }
}
