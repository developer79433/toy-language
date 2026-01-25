#ifndef TOY_ITER_TYPES_H
#define TOY_ITER_TYPES_H 1

typedef enum item_callback_result_enum {
    STOP_ENUMERATION = 1,
    CONTINUE_ENUMERATION = 2
} item_callback_result;

typedef enum enumeration_result_enum {
    ENUMERATION_COMPLETE = 3,
    ENUMERATION_INTERRUPTED = 4
} enumeration_result;

typedef enum get_result_enum {
    GET_NOT_FOUND = 5,
    GET_FOUND = 6
} get_result;

typedef enum set_result_enum {
    SET_NEW = 7,
    SET_EXISTING = 8
} set_result;

typedef enum delete_result_enum {
    NOT_PRESENT = 9,
    DELETED = 10
} delete_result;

#endif /* TOY_ITER_TYPES_H */
