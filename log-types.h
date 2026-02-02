#ifndef TOY_LOG_TYPES_H
#define TOY_LOG_TYPES_H 1

typedef enum log_level_enum {
    LOG_DEBUG = 0,
    LOG_INFO  = 1,
    LOG_WARN  = 2,
    LOG_ERROR = 3
} log_level;

#define LOG_LEVEL_MAX LOG_ERROR

#endif /* TOY_LOG_TYPES_H */
