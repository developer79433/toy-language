#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "log.h"
#include "str.h"
#include "generic-map.h"
#include "mymalloc.h"
#include "errors.h"
#include "generic-map-entry-list.h"
#include "map-visitor.h"
#include "map-filter.h"
#include "map-latch.h"

void map_init(generic_map *map)
{
    memset(map->buckets, 0, sizeof(map->buckets));
    map->num_items = 0;
}

generic_map *map_alloc(void)
{
    generic_map *map = mymalloc(generic_map);
    map_init(map);
    return map;
}

enumeration_result bucket_visitor_visit_map(bucket_visitor *visitor, generic_map *map)
{
    /* TODO: Push this down into an array enumerator */
    for (generic_map_entry_list * const * pbucket = &map->buckets[0]; pbucket < &map->buckets[NUM_BUCKETS]; pbucket++) {
        assert(pbucket);
        generic_map_entry_list *bucket = *pbucket;
        if (bucket) {
            item_callback_result res = visitor->visit_bucket(visitor, bucket);
            if (res == STOP_ENUMERATION) {
                return ENUMERATION_INTERRUPTED;
            }
        }
    }
    return ENUMERATION_COMPLETE;
}

enumeration_result const_bucket_visitor_visit_map(const_bucket_visitor *visitor, const generic_map *map)
{
    /* TODO: Push this down into an array enumerator */
    for (generic_map_entry_list * const * pbucket = &map->buckets[0]; pbucket < &map->buckets[NUM_BUCKETS]; pbucket++) {
        assert(pbucket);
        const generic_map_entry_list *bucket = *pbucket;
        if (bucket) {
            item_callback_result res = visitor->visit_bucket(visitor, bucket);
            if (res == STOP_ENUMERATION) {
                return ENUMERATION_INTERRUPTED;
            }
        }
    }
    return ENUMERATION_COMPLETE;
}

static item_callback_result bucket_free_cb(bucket_visitor *visitor, generic_map_entry_list *bucket)
{
    generic_map_entry_list_free(bucket);
    return CONTINUE_ENUMERATION;
}

static void free_buckets(generic_map *map)
{
    bucket_visitor free_visitor = { .visit_bucket = bucket_free_cb };
    enumeration_result res = bucket_visitor_visit_map(&free_visitor, map);
    assert(ENUMERATION_COMPLETE == res);
    memset(map->buckets, 0, sizeof(map->buckets));
    map->num_items = 0;
}

void map_reset(generic_map *map)
{
    free_buckets(map);
}

void map_free(generic_map *map)
{
    free_buckets(map);
    free(map);
}

static uint32_t jenkins_one_at_a_time_hash(const uint8_t *key, size_t length) {
    size_t i = 0;
    uint32_t hash = 0;
    while (i != length) {
        hash += key[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}

generic_map_entry_list **map_get_bucket_ptr(generic_map *map, toy_str key)
{
    uint32_t hashval = jenkins_one_at_a_time_hash((uint8_t *) key, strlen(key));
    return &map->buckets[hashval % NUM_BUCKETS];
}

generic_map_entry_list *generic_map_get_bucket(generic_map *map, toy_str key)
{
    generic_map_entry_list **bucket_ptr = map_get_bucket_ptr(map, key);
    return *bucket_ptr;
}

typedef struct map_entry_has_name_args_struct {
    toy_str desired_key;
} map_entry_has_name_args;

static toy_bool map_entry_has_desired_name(void *cookie, const generic_map_entry *entry)
{
    map_entry_has_name_args *args = (map_entry_has_name_args *) cookie;
    return str_equal(entry->key, args->desired_key);
}

typedef struct delete_cb_args_struct {
    generic_map *map;
    generic_map_filter_func filter_func;
    map_entry_has_name_args filter_func_arg;
    generic_map_entry_list **bucket;
    generic_map_entry_list *prev;
} delete_cb_args;

static item_callback_result delete_first_callback(void *cookie, size_t index, generic_map_entry_list *list)
{
    delete_cb_args *args = (delete_cb_args *) cookie;
    generic_map_entry *map_entry = generic_map_entry_list_payload(list);
    item_callback_result res;
    if (args->filter_func(&args->filter_func_arg, map_entry)) {
        /* Found existing entry */
        args->prev->next = list->next;
        if (list == *(args->bucket)) {
            assert(list->next == NULL);
            *(args->bucket) = NULL;
        } else {
            list->next = NULL;
        }
        generic_map_entry_list_free(list);
        args->map->num_items--;
        res = STOP_ENUMERATION;
    } else {
        res = CONTINUE_ENUMERATION;
    }
    args->prev = list;
    return res;
}

/* TODO: Push this down into generic_list */
static delete_result delete_from_bucket(generic_map *map, generic_map_entry_list **bucket, const toy_str key)
{
    map_entry_has_name_args has_name_args = { .desired_key = key };
    delete_cb_args delete_args = { .map = map, .filter_func = map_entry_has_desired_name, .filter_func_arg = has_name_args, .bucket = bucket, .prev = *bucket };
    enumeration_result res = generic_map_entry_list_foreach(*bucket, delete_first_callback, &delete_args);
    if (res == ENUMERATION_COMPLETE) {
        return NOT_PRESENT;
    }
    assert(res == ENUMERATION_INTERRUPTED);
    return DELETED;
}

delete_result map_delete(generic_map *map, const toy_str key)
{
    generic_map_entry_list **bucket = map_get_bucket_ptr(map, key);
    if (*bucket) {
        return delete_from_bucket(map, bucket, key);
    }
    return NOT_PRESENT; /* No bucket, so no entry */
}

static void dump_map_entry(const generic_map_entry *entry)
{
    str_dump(entry->key);
    log_printf(": %p", entry + 1);
}

typedef struct map_dumper_struct {
    const_map_visitor visitor;
    int output_anything;
} map_dumper;

static item_callback_result dump_item_callback(map_dumper *dumper, const generic_map_entry *entry)
{
    if (dumper->output_anything) {
        log_puts(", ");
    } else {
        log_putc(' ');
    }
    dump_map_entry(entry);
    dumper->output_anything = TOY_TRUE;
    return CONTINUE_ENUMERATION;
}

void map_dump(const generic_map *map)
{
    map_dumper dumper = { .visitor.visit_entry = (const_map_entry_visit_func) dump_item_callback, .output_anything = TOY_FALSE };
    log_putc('{');
    const_map_visitor_visit_map((const_map_visitor *) &dumper, map);
    if (dumper.output_anything) {
        log_putc(' ');
    }
    log_putc('}');
}

typedef struct map_item_cb_struct {
    generic_map_entry_callback cb;
    void *cookie;
} map_item_cb;

static item_callback_result map_foreach_item_cb(void *cookie, size_t index, generic_map_entry_list *list)
{
    map_item_cb *item_cb = (map_item_cb *) cookie;
    generic_map_entry *entry = generic_map_entry_list_payload(list);
    return item_cb->cb(item_cb->cookie, entry);
}

typedef struct foreach_bucket_visitor_struct {
    bucket_visitor visitor;
    map_item_cb item_cb;
} foreach_bucket_visitor;

static item_callback_result map_foreach_bucket_cb(foreach_bucket_visitor *visitor, generic_map_entry_list *bucket)
{
    /* TODO: Use visitors */
    enumeration_result enum_res = generic_map_entry_list_foreach(bucket, map_foreach_item_cb, &visitor->item_cb);
    if (enum_res == ENUMERATION_INTERRUPTED) {
        return STOP_ENUMERATION;
    }
    assert(ENUMERATION_COMPLETE == enum_res);
    return CONTINUE_ENUMERATION;
}

enumeration_result map_foreach(generic_map *map, generic_map_entry_callback callback, void *cookie)
{
    foreach_bucket_visitor visitor = {
        .visitor.visit_bucket = (bucket_visit_func) map_foreach_bucket_cb,
        .item_cb.cb = callback,
        .item_cb.cookie = cookie
    };
    return bucket_visitor_visit_map((bucket_visitor *) &visitor, map);
}

typedef struct const_map_item_cb_struct {
    const_generic_map_entry_callback cb;
    void *cookie;
} const_map_item_cb;

static item_callback_result const_map_foreach_item_cb(void *cookie, size_t index, const generic_map_entry_list *list)
{
    const_map_item_cb *item_cb = (const_map_item_cb *) cookie;
    const generic_map_entry *entry = generic_map_entry_list_payload_const(list);
    return item_cb->cb(item_cb->cookie, entry);
}

typedef struct const_foreach_bucket_visitor_struct {
    const_bucket_visitor visitor;
    const_map_item_cb item_cb;
} const_foreach_bucket_visitor;

static item_callback_result const_map_foreach_bucket_cb(const_foreach_bucket_visitor *visitor, const generic_map_entry_list *bucket)
{
    /* TODO: Use visitors */
    enumeration_result enum_res = generic_map_entry_list_foreach_const(bucket, const_map_foreach_item_cb, &visitor->item_cb);
    if (enum_res == ENUMERATION_INTERRUPTED) {
        return STOP_ENUMERATION;
    }
    assert(ENUMERATION_COMPLETE == enum_res);
    return CONTINUE_ENUMERATION;
}

enumeration_result map_foreach_const(const generic_map *map, const_generic_map_entry_callback callback, void *cookie)
{
    const_foreach_bucket_visitor visitor = {
        .visitor.visit_bucket = (const_bucket_visit_func) const_map_foreach_bucket_cb,
        .item_cb.cb = callback,
        .item_cb.cookie = cookie
    };
    return const_bucket_visitor_visit_map((const_bucket_visitor *) &visitor, map);
}

typedef struct listentry_cb_args_struct {
    toy_str desired_name;
    generic_map_entry *entry_to_find;
} listentry_cb_args;

static item_callback_result map_get_listentry_cb(void *cookie, size_t index, generic_map_entry_list *list)
{
    listentry_cb_args *args = (listentry_cb_args *) cookie;
    generic_map_entry *map_entry = generic_map_entry_list_payload(list);
    if (str_equal(map_entry->key, args->desired_name)) {
        args->entry_to_find = map_entry;
        return STOP_ENUMERATION;
    }
    return CONTINUE_ENUMERATION;
}

static generic_map_entry *map_bucket_get_key(generic_map_entry_list *bucket, const toy_str key)
{
    /* TODO: Use visitors */
    listentry_cb_args args = { .desired_name = key, .entry_to_find = NULL };
    enumeration_result res = generic_map_entry_list_foreach(bucket, map_get_listentry_cb, &args);
    assert(
        (res == ENUMERATION_COMPLETE && args.entry_to_find == NULL)
        ||
        (res = ENUMERATION_INTERRUPTED && args.entry_to_find != NULL)
    );
    return args.entry_to_find;
}

generic_map_entry *map_get_entry(generic_map *map, const toy_str key)
{
    generic_map_entry_list *bucket = generic_map_get_bucket(map, key);
    if (bucket) {
        generic_map_entry *existing_entry = map_bucket_get_key(bucket, key);
        if (existing_entry) {
            assert(str_equal(existing_entry->key, key));
            return existing_entry;
        }
        return NULL;
    }
    return NULL;
}

size_t map_size(const generic_map *map)
{
    return map->num_items;
}

void map_assert_valid(const generic_map *map)
{
    assert(map);
    /* TODO */
}

const generic_map_entry *map_get_entry_const(const generic_map *map, const toy_str key)
{
    return (const generic_map_entry *) map_get_entry((generic_map *) map, key);
}

generic_map_entry *map_find(generic_map *map, generic_map_filter_func filter_func, void *filter_cookie, toy_bool inverted, toy_bool stop_on_first)
{
    map_filter filter;
    map_latch latch;
    map_latch_init(&latch, stop_on_first);
    map_filter_init(&filter, filter_func, filter_cookie, inverted, (map_visitor *) &latch);
    enumeration_result res = map_visitor_visit_map((map_visitor *) &filter, map);
    assert(
        (res == ENUMERATION_COMPLETE && map_latch_get_last_seen(&latch) == NULL)
        ||
        (res == ENUMERATION_INTERRUPTED && map_latch_get_last_seen(&latch) != NULL)
    );
    return map_latch_get_last_seen(&latch);
}

generic_map_entry *map_find_first(generic_map *map, generic_map_filter_func filter_func, void *filter_cookie)
{
    return map_find(map, filter_func, filter_cookie, TOY_FALSE, TOY_TRUE);
}

generic_map_entry *map_find_first_not(generic_map *map, generic_map_filter_func filter_func, void *filter_cookie)
{
    return map_find(map, filter_func, filter_cookie, TOY_TRUE, TOY_TRUE);
}

generic_map_entry *map_find_last(generic_map *map, generic_map_filter_func filter_func, void *filter_cookie)
{
    return map_find(map, filter_func, filter_cookie, TOY_FALSE, TOY_FALSE);
}

generic_map_entry *map_find_last_not(generic_map *map, generic_map_filter_func filter_func, void *filter_cookie)
{
    return map_find(map, filter_func, filter_cookie, TOY_TRUE, TOY_FALSE);
}

const generic_map_entry *map_find_const(const generic_map *map, generic_map_filter_func filter_func, void *filter_cookie, toy_bool inverted, toy_bool stop_on_first)
{
    const_map_filter filter;
    map_latch latch;
    map_latch_init(&latch, stop_on_first);
    const_map_filter_init(&filter, filter_func, filter_cookie, inverted, (const_map_visitor *) &latch);
    enumeration_result res = const_map_visitor_visit_map((const_map_visitor *) &filter, map);
    assert(
        (res == ENUMERATION_COMPLETE && map_latch_get_last_seen(&latch) == NULL)
        ||
        (res == ENUMERATION_INTERRUPTED && map_latch_get_last_seen(&latch) != NULL)
    );
    return map_latch_get_last_seen(&latch);
}

const generic_map_entry *map_find_first_const(const generic_map *map, generic_map_filter_func filter_func, void *filter_cookie)
{
    return map_find_const(map, filter_func, filter_cookie, TOY_FALSE, TOY_TRUE);
}

const generic_map_entry *map_find_first_not_const(const generic_map *map, generic_map_filter_func filter_func, void *filter_cookie)
{
    return map_find_const(map, filter_func, filter_cookie, TOY_TRUE, TOY_TRUE);
}

const generic_map_entry *map_find_last_const(const generic_map *map, generic_map_filter_func filter_func, void *filter_cookie)
{
    return map_find_const(map, filter_func, filter_cookie, TOY_FALSE, TOY_FALSE);
}

const generic_map_entry *map_find_last_not_const(const generic_map *map, generic_map_filter_func filter_func, void *filter_cookie)
{
    return map_find_const(map, filter_func, filter_cookie, TOY_TRUE, TOY_FALSE);
}

toy_bool map_all_match(const generic_map *map, generic_map_filter_func filter, void *cookie)
{
    const generic_map_entry *entry = map_find_first_not_const(map, filter, cookie);
    return entry == NULL;
}

toy_bool map_none_match(const generic_map *map, generic_map_filter_func filter, void *cookie)
{
    const generic_map_entry *entry = map_find_first_const(map, filter, cookie);
    return entry == NULL;
}

toy_bool map_not_all_match(const generic_map *map, generic_map_filter_func filter, void *cookie)
{
    return !map_all_match(map, filter, cookie);
}

toy_bool map_some_match(const generic_map *map, generic_map_filter_func filter, void *cookie)
{
    return !map_none_match(map, filter, cookie);
}

typedef struct dump_keys_visitor_struct {
    item_callback_result (*visit)(struct const_map_visitor_struct *visitor, const generic_map_entry *entry);
    toy_bool output_anything;
} dump_keys_visitor;

static item_callback_result dump_keys_cb(dump_keys_visitor *visitor, const generic_map_entry *entry)
{
    if (visitor->output_anything) {
        log_puts(", ");
    } else {
        log_putc(' ');
    }
    str_dump(entry->key);
    visitor->output_anything = TOY_TRUE;
    return CONTINUE_ENUMERATION;
}

void map_dump_keys(const generic_map *map)
{
    log_putc('[');
    dump_keys_visitor visitor = { .visit = (const_map_entry_visit_func) dump_keys_cb, .output_anything = TOY_FALSE };
    enumeration_result res = const_map_visitor_visit_map((const_map_visitor *) &visitor, map);
    assert(ENUMERATION_COMPLETE == res);
    if (visitor.output_anything) {
        log_putc(' ');
    }
    log_putc(']');
}
