#ifndef GENERIC_LIST_TYPES_H
#define GENERIC_LIST_TYPES_H 1

typedef struct generic_list_struct {
    struct generic_list_struct *next;
    void *payload;
} generic_list;

#endif /* GENERIC_LIST_TYPES_H */
