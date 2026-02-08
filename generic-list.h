#ifndef GENERIC_LIST_H
#define GENERIC_LIST_H 1

#include <stddef.h>

#include "num-types.h"
#include "generic-list-types.h"

generic_list *list_next(generic_list *list);
const generic_list *list_next_const(const generic_list *list);
generic_list *list_alloc_size(size_t value_size);
generic_list *list_concat(generic_list *orig_list, generic_list *new_list);
generic_list *list_remove_first(generic_list *list, generic_list **removed);
generic_list *list_remove_last(generic_list *list, generic_list **removed);
void list_free(generic_list *list);
generic_list *list_index(generic_list *list, size_t index);
const generic_list *list_index_const(const generic_list *list, size_t index);
size_t list_len(const generic_list *list);
generic_list *list_last(generic_list *list, generic_list **prev);
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
generic_list *list_delete_first(generic_list *list, generic_list_filter_func filter_func, void *filter_cookie, list_entry_free_func free_func, delete_result *del_res);
generic_list *list_delete_first_not(generic_list *list, generic_list_filter_func filter_func, void *filter_cookie, list_entry_free_func free_func, delete_result *del_res);
generic_list *list_delete_last(generic_list *list, generic_list_filter_func filter_func, void *filter_cookie, list_entry_free_func free_func, delete_result *del_res);
generic_list *list_delete_last_not(generic_list *list, generic_list_filter_func filter_func, void *filter_cookie, list_entry_free_func free_func, delete_result *del_res);
generic_list *list_delete_filter(generic_list *list, generic_list_filter_func filter_func, void *filter_cookie, toy_bool stop_on_first, toy_bool inverted, list_entry_free_func free_func);
void list_assert_all_match(const generic_list *list, generic_list_filter_func filter_func, void *filter_cookie);
void list_assert_none_match(const generic_list *list, generic_list_filter_func filter_func, void *filter_cookie);
void list_assert_not_all_match(const generic_list *list, generic_list_filter_func filter_func, void *filter_cookie);
void list_assert_some_match(const generic_list *list, generic_list_filter_func filter_func, void *filter_cookie);
toy_bool list_contains(const generic_list *list, const generic_list *entry);
void list_assert_contains(const generic_list *list, const generic_list *entry);

#endif /* GENERIC_LIST_H */
