#ifndef TOY_PTR_LIST_H
#define TOY_PTR_LIST_H 1

#include "ptr-list-types.h"

toy_ptr_list *ptr_list_alloc(void *ptr);
toy_ptr_list *ptr_list_append(toy_ptr_list *list, void *new_ptr);
toy_ptr_list *ptr_list_concat(toy_ptr_list *list, toy_ptr_list *new_list);
list_iter_result ptr_list_index(toy_ptr_list *list, size_t index, ptr_list_item_callback callback, void *cookie);
size_t ptr_list_len(const toy_ptr_list *list);
list_iter_result ptr_list_foreach(toy_ptr_list *list, ptr_list_item_callback callback, void *cookie);
list_iter_result ptr_list_foreach_const(const toy_ptr_list *list, const_ptr_list_item_callback callback, void *cookie);
void ptr_list_free(toy_ptr_list *list);

#endif /* TOY_PTR_LIST_H */
