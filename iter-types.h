#ifndef TOY_ITER_TYPES_H
#define TOY_ITER_TYPES_H 1

typedef enum item_callback_result_enum {
    STOP_ENUMERATION = 0,
    CONTINUE_ENUMERATION = 1
} item_callback_result;

typedef enum enumeration_result_enum {
    ENUMERATION_COMPLETE = 0,
    ENUMERATION_INTERRUPTED = 1
} enumeration_result;

typedef enum get_result_enum {
    GET_NOT_FOUND = 0,
    GET_FOUND = 1
} get_result;

typedef enum set_result_enum {
    SET_NEW = 0,
    SET_EXISTING = 1
} set_result;

/* TODO: Re-order these */
typedef enum delete_result_enum {
    DELETED = 0,
    NOT_PRESENT = 1
} delete_result;

#endif /* TOY_ITER_TYPES_H */
