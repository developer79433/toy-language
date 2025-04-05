#ifndef TOY_VAL_LIST_TYPES_H
#define TOY_VAL_LIST_TYPES_H 1

struct toy_val_struct;
typedef struct toy_val_struct toy_val;

typedef struct toy_val_list_struct {
    toy_val *val;
    struct toy_val_list_struct *next;
} toy_val_list;

#endif /* VAL_TOY_LIST_TYPES_H */
