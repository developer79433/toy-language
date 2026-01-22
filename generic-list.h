#ifndef GENERIC_LIST_H
#define GENERIC_LIST_H 1

#include <stddef.h>

#include "num-types.h"
#include "generic-list-types.h"

generic_list *generic_list_next(generic_list *list);
const generic_list *generic_list_next_const(const generic_list *list);
generic_list *generic_list_alloc_size(size_t value_size);
generic_list *generic_list_concat(generic_list *orig_list, generic_list *new_list);
generic_list *generic_list_remove_first(generic_list *list, generic_list **removed);
generic_list *generic_list_remove_last(generic_list *list, generic_list **removed);
enumeration_result generic_list_foreach(generic_list *list, generic_list_item_callback callback, void *cookie);
enumeration_result generic_list_foreach_const(const generic_list *list, const_generic_list_item_callback callback, void *cookie);
void generic_list_free(generic_list *list);
generic_list *generic_list_index(generic_list *list, size_t index);
const generic_list *generic_list_index_const(const generic_list *list, size_t index);
size_t generic_list_len(const generic_list *list);
generic_list *generic_list_last(generic_list *list, generic_list **prev);
generic_list *list_find_first(generic_list *list, generic_list_filter_func filter_func, void *filter_cookie, generic_list **prev);
generic_list *list_find_first_not(generic_list *list, generic_list_filter_func filter_func, void *filter_cookie, generic_list **prev);
generic_list *list_find_last(generic_list *list, generic_list_filter_func filter_func, void *filter_cookie, generic_list **prev);
generic_list *list_find_last_not(generic_list *list, generic_list_filter_func filter_func, void *filter_cookie, generic_list **prev);
const generic_list *list_find_first_const(const generic_list *list, generic_list_filter_func filter_func, void *filter_cookie);
const generic_list *list_find_first_not_const(const generic_list *list, generic_list_filter_func filter_func, void *filter_cookie);
const generic_list *list_find_last_const(const generic_list *list, generic_list_filter_func filter_func, void *filter_cookie);
const generic_list *list_find_last_not_const(const generic_list *list, generic_list_filter_func filter_func, void *filter_cookie);
//  Ax  m(x)
// ~Ex ~m(x)
toy_bool list_all_match(const generic_list *list, generic_list_filter_func filter_func, void *filter_cookie);
//  Ax ~m(x)
// ~Ex  m(x)
toy_bool list_none_match(const generic_list *list, generic_list_filter_func filter_func, void *filter_cookie);
// ~Ax  m(x)
//  Ex ~m(x)
toy_bool list_not_all_match(const generic_list *list, generic_list_filter_func filter_func, void *filter_cookie);
// ~Ax ~m(x)
//  Ex  m(x)
toy_bool list_some_match(const generic_list *list, generic_list_filter_func filter_func, void *filter_cookie);
toy_bool match_always(void *cookie, size_t index, const generic_list *item);
toy_bool match_never(void *cookie, size_t index, const generic_list *item);

#endif /* GENERIC_LIST_H */
