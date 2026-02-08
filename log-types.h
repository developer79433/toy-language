#ifndef TOY_LOG_TYPES_H
#define TOY_LOG_TYPES_H 1

typedef enum log_level_enum {
    LOG_DEBUG = 1,
    LOG_INFO  = 2,
    LOG_WARN  = 3,
    LOG_ERROR = 4
} log_level;

#define LOG_LEVEL_MIN LOG_DEBUG
#define LOG_LEVEL_MAX LOG_ERROR

#endif /* TOY_LOG_TYPES_H */
