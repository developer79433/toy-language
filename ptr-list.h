#ifndef TOY_PTR_LIST_H
#define TOY_PTR_LIST_H 1

#include "ptr-list-types.h"

toy_ptr_list *ptr_list_alloc(void *ptr);
toy_ptr_list *ptr_list_append(toy_ptr_list *list, void *new_ptr);
toy_ptr_list *ptr_list_concat(toy_ptr_list *list, toy_ptr_list *new_list);
void *ptr_list_index(toy_ptr_list *list, size_t index);
size_t ptr_list_len(const toy_ptr_list *list);
enumeration_result ptr_list_foreach(toy_ptr_list *list, ptr_list_item_callback callback, void *cookie);
enumeration_result ptr_list_foreach_const(const toy_ptr_list *list, const_ptr_list_item_callback callback, void *cookie);
void *ptr_list_payload(toy_ptr_list *list);
const void *ptr_list_payload_const(const toy_ptr_list *list);
void ptr_list_set_payload(toy_ptr_list *list, void *new_payload);
#define ptr_list_payload_typed(list, type) ((type *) ptr_list_payload(list))
#define ptr_list_payload_const_typed(list, type) ((const type *) ptr_list_payload_const(list))
void ptr_list_free(toy_ptr_list *list);

#endif /* TOY_PTR_LIST_H */
