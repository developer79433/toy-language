#ifndef TOY_LOG_H
#define TOY_LOG_H 1

#include <stdarg.h>

void log_debug(const char *str);
int log_vprintf(const char *fmt, va_list argptr);
int log_printf_file(const char *filename, const char *fmt, ...);
int log_printf(const char *fmt, ...);
int log_putc(int c);
int log_puts(const char *str);

#endif /* TOY_LOG_H */
