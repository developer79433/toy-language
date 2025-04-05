#ifndef TOY_VAL_LIST_TYPES_H
#define TOY_VAL_LIST_TYPES_H 1

struct toy_val_struct;
typedef struct toy_val_struct toy_val;

typedef struct toy_val_list_struct {
    struct toy_val_list_struct *next;
    toy_val *val;
} toy_val_list;

typedef void (*val_list_item_callback)(void *cookie, toy_val *val);
typedef void (*const_val_list_item_callback)(void *cookie, const toy_val *val);

#endif /* VAL_TOY_LIST_TYPES_H */
