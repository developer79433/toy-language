#ifndef TOY_GENERIC_MAP_H
#define TOY_GENERIC_MAP_H 1

#include <stddef.h>

#include "generic-map-types.h"
#include "str-types.h"

void map_init(generic_map *map);
generic_map *map_alloc(void);
generic_map_entry_list **map_get_bucket_ptr(generic_map *map, toy_str key);
enumeration_result bucket_visitor_visit_map(bucket_visitor *visitor, generic_map *map);
enumeration_result const_bucket_visitor_visit_map(const_bucket_visitor *visitor, const generic_map *map);
void map_free(generic_map *map);
generic_map_entry *map_get_entry(generic_map *map, const toy_str key);
const generic_map_entry *map_get_entry_const(const generic_map *map, const toy_str key);
delete_result map_delete(generic_map *map, const toy_str key);
enumeration_result map_foreach(generic_map *map, generic_map_entry_callback callback, void *cookie);
enumeration_result map_foreach_const(const generic_map *map, const_generic_map_entry_callback callback, void *cookie);
generic_map_entry *map_find(generic_map *map, generic_map_filter_func filter, void *cookie, toy_bool inverted, toy_bool stop_on_first);
generic_map_entry *map_find_first(generic_map *map, generic_map_filter_func filter_func, void *filter_cookie);
generic_map_entry *map_find_first_not(generic_map *map, generic_map_filter_func filter_func, void *filter_cookie);
generic_map_entry *map_find_last(generic_map *map, generic_map_filter_func filter_func, void *filter_cookie);
generic_map_entry *map_find_last_not(generic_map *map, generic_map_filter_func filter_func, void *filter_cookie);
const generic_map_entry *map_find_const(const generic_map *map, generic_map_filter_func filter, void *cookie, toy_bool inverted, toy_bool stop_on_first);
const generic_map_entry *map_find_first_const(const generic_map *map, generic_map_filter_func filter_func, void *filter_cookie);
const generic_map_entry *map_find_first_not_const(const generic_map *map, generic_map_filter_func filter_func, void *filter_cookie);
const generic_map_entry *map_find_last_const(const generic_map *map, generic_map_filter_func filter_func, void *filter_cookie);
const generic_map_entry *map_find_last_not_const(const generic_map *map, generic_map_filter_func filter_func, void *filter_cookie);
/* TODO: Should be const */
toy_bool map_all_match(generic_map *map, generic_map_filter_func filter, void *cookie);
toy_bool map_none_match(generic_map *map, generic_map_filter_func filter, void *cookie);
toy_bool map_not_all_match(generic_map *map, generic_map_filter_func filter, void *cookie);
toy_bool map_some_match(generic_map *map, generic_map_filter_func filter, void *cookie);
void map_dump(const generic_map *map);
void map_dump_keys(const generic_map *map);
size_t map_size(const generic_map *map);
void map_reset(generic_map *map);
void map_assert_valid(const generic_map *map);

#endif /* TOY_GENERIC_MAP_H */
