#ifndef GENERIC_LIST_TYPES_H
#define GENERIC_LIST_TYPES_H 1

typedef struct generic_list_struct {
    struct generic_list_struct *next;
    /* TODO: Keep track of length? */
    void *payload;
} generic_list;

typedef void (*list_item_callback)(void *cookie, void *item);
typedef void (*const_list_item_callback)(void *cookie, const void *item);

extern void *INDEX_OUT_OF_BOUNDS;

#endif /* GENERIC_LIST_TYPES_H */
