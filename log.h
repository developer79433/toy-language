#ifndef TOY_LOG_H
#define TOY_LOG_H 1

#include <stddef.h>
#include <stdarg.h>

#include "str-types.h"
#include "log-types.h"

#define log_file(level, ...) do { log_printf(level, L"%s:%-4u: ", __FILE__, __LINE__); log_printf(level, __VA_ARGS__); } while (0)
#define log_debug_file(...) log_file(LOG_DEBUG, __VA_ARGS__)
#define log_info_file(...)  log_file(LOG_INFO,  __VA_ARGS__)
#define log_warn_file(...)  log_file(LOG_WARN,  __VA_ARGS__)
#define log_error_file(...) log_file(LOG_ERROR, __VA_ARGS__)

const toy_str log_level_name(log_level level);
void log_set_level(log_level new_level);
log_level log_get_level(void);
void log_to_file(const toy_str filename);
void log_putc(log_level level, toy_char c);
void log_puts(log_level level, const toy_str str);
void log_vprintf(log_level level, const toy_str fmt, va_list argptr);
void log_printf(log_level level, const toy_str fmt, ...);
void log_debug(const toy_str fmt, ...);
void log_info(const toy_str fmt, ...);
void log_warn(const toy_str fmt, ...);
void log_error(const toy_str fmt, ...);
void hex_dump(const void *ptr, size_t len);

#endif /* TOY_LOG_H */
