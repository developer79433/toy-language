#ifndef TOY_ITER_TYPES_H
#define TOY_ITER_TYPES_H 1

typedef enum item_callback_result_enum {
    STOP_ENUMERATION = 0,
    CONTINUE_ENUMERATION = 1
} item_callback_result;

typedef enum enumeration_result_enum {
    ENUMERATION_COMPLETE = 0,
    EUMERATION_INTERRUPTED = 1
} enumeration_result;

#endif /* TOY_ITER_TYPES_H */
