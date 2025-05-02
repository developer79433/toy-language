#ifndef TOY_ITER_TYPES_H
#define TOY_ITER_TYPES_H 1

typedef enum item_callback_result_enum {
    STOP_ITERATING = 0,
    CONTINUE_ITERATING = 1
} item_callback_result;

typedef enum iter_result_enum {
    ENUMERATION_COMPLETE = 0,
    EUMERATION_INTERRUPTED = 1
} iter_result;

#endif /* TOY_ITER_TYPES_H */
