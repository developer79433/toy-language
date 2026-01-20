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

void generic_map_init(generic_map *map)
{
    memset(map->buckets, 0, sizeof(map->buckets));
    map->num_items = 0;
}

generic_map *generic_map_alloc(void)
{
    generic_map *map = mymalloc(generic_map);
    generic_map_init(map);
    return map;
}

typedef item_callback_result (*generic_map_bucket_callback)(void *cookie, generic_map_entry_list *bucket);

static enumeration_result generic_map_enum_buckets(generic_map *map, generic_map_bucket_callback callback, void *cookie)
{
    /* TODO: Push this down into an array enumerator */
    for (generic_map_entry_list * const * pbucket = &map->buckets[0]; pbucket < &map->buckets[NUM_BUCKETS]; pbucket++) {
        assert(pbucket);
        generic_map_entry_list *bucket = *pbucket;
        if (bucket) {
            item_callback_result res = callback(cookie, bucket);
            if (res == STOP_ENUMERATION) {
                return ENUMERATION_INTERRUPTED;
            }
        }
    }
    return ENUMERATION_COMPLETE;
}

typedef item_callback_result (*const_generic_map_bucket_callback)(void *cookie, const generic_map_entry_list *bucket);

static enumeration_result generic_map_enum_buckets_const(const generic_map *map, const_generic_map_bucket_callback callback, void *cookie)
{
    /* TODO: Push this down into an array enumerator */
    for (generic_map_entry_list * const * pbucket = &map->buckets[0]; pbucket < &map->buckets[NUM_BUCKETS]; pbucket++) {
        assert(pbucket);
        const generic_map_entry_list *bucket = *pbucket;
        if (bucket) {
            item_callback_result res = callback(cookie, bucket);
            if (res == STOP_ENUMERATION) {
                return ENUMERATION_INTERRUPTED;
            }
        }
    }
    return ENUMERATION_COMPLETE;
}

item_callback_result bucket_free_cb(void *cookie, generic_map_entry_list *bucket)
{
    generic_map_entry_list_free(bucket);
    return CONTINUE_ENUMERATION;
}

static void free_buckets(generic_map *map)
{
    generic_map_enum_buckets(map, bucket_free_cb, NULL);
    memset(map->buckets, 0, sizeof(map->buckets));
    map->num_items = 0;
}

void generic_map_reset(generic_map *map)
{
    free_buckets(map);
}

void generic_map_free(generic_map *map)
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

generic_map_entry_list **generic_map_get_bucket_ptr(generic_map *map, toy_str key)
{
    uint32_t hashval = jenkins_one_at_a_time_hash((uint8_t *) key, strlen(key));
    return &map->buckets[hashval % NUM_BUCKETS];
}

generic_map_entry_list *generic_map_get_bucket(generic_map *map, toy_str key)
{
    generic_map_entry_list **bucket_ptr = generic_map_get_bucket_ptr(map, key);
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

delete_result generic_map_delete(generic_map *map, const toy_str key)
{
    generic_map_entry_list **bucket = generic_map_get_bucket_ptr(map, key);
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

typedef struct dump_item_cb_args_struct {
    int output_anything;
} dump_item_cb_args;

static item_callback_result dump_item_callback(void *cookie, const generic_map_entry *entry)
{
    dump_item_cb_args *args = (dump_item_cb_args *) cookie;
    if (args->output_anything) {
        log_puts(", ");
    } else {
        log_putc(' ');
    }
    dump_map_entry(entry);
    args->output_anything = 1;
    return CONTINUE_ENUMERATION;
}

void generic_map_dump(const generic_map *map)
{
    dump_item_cb_args cbargs = { .output_anything = 0 };
    log_putc('{');
    generic_map_foreach_const(map, dump_item_callback, &cbargs);
    if (cbargs.output_anything) {
        log_putc(' ');
    }
    log_putc('}');
}

typedef struct buflist_item_cb_args_struct {
    generic_map_entry_callback mapitem_cb;
    void *mapitem_cookie;
} buflist_item_cb_args;

static item_callback_result buflist_item_cb(void *cookie, size_t index, generic_map_entry_list *list)
{
    buflist_item_cb_args *args = (buflist_item_cb_args *) cookie;
    generic_map_entry *entry = generic_map_entry_list_payload(list);
    return args->mapitem_cb(args->mapitem_cookie, entry);
}

typedef struct bucket_cb_args_struct {
    buflist_item_cb_args buflist_item_args;
    generic_map_entry_list_item_callback buflist_item_cb;
} bucket_cb_args;

static item_callback_result generic_map_foreach_bucket_cb(void *cookie, generic_map_entry_list *bucket)
{
    bucket_cb_args *args = (bucket_cb_args *) cookie;
    enumeration_result enum_res = generic_map_entry_list_foreach(bucket, args->buflist_item_cb, &args->buflist_item_args);
    if (enum_res == ENUMERATION_INTERRUPTED) {
        return STOP_ENUMERATION;
    }
    assert(ENUMERATION_COMPLETE == enum_res);
    return CONTINUE_ENUMERATION;
}

enumeration_result generic_map_foreach(generic_map *map, generic_map_entry_callback callback, void *cookie)
{
    buflist_item_cb_args buflist_item_args = { .mapitem_cb = callback, .mapitem_cookie = cookie };
    bucket_cb_args bucket_args = { .buflist_item_args = buflist_item_args, .buflist_item_cb = buflist_item_cb };
    return generic_map_enum_buckets(map, generic_map_foreach_bucket_cb, &bucket_args);
}

typedef struct const_buflist_item_cb_args_struct {
    const_generic_map_entry_callback item_cb;
    void *item_cb_cookie;
} const_buflist_item_cb_args;

static item_callback_result const_buflist_item_cb(void *cookie, size_t index, const generic_map_entry_list *list)
{
    const_buflist_item_cb_args *args = (const_buflist_item_cb_args *) cookie;
    const generic_map_entry *entry = generic_map_entry_list_payload_const(list);
    return args->item_cb(args->item_cb_cookie, entry);
}

typedef struct const_bucket_cb_args_struct {
    const_buflist_item_cb_args item_cb_args;
} const_bucket_cb_args;

static item_callback_result const_generic_map_foreach_bucket_cb(void *cookie, const generic_map_entry_list *bucket)
{
    const_bucket_cb_args *args = (const_bucket_cb_args *) cookie;
    enumeration_result enum_res = generic_map_entry_list_foreach_const(bucket, const_buflist_item_cb, &args->item_cb_args);
    if (enum_res == ENUMERATION_INTERRUPTED) {
        return STOP_ENUMERATION;
    }
    assert(ENUMERATION_COMPLETE == enum_res);
    return CONTINUE_ENUMERATION;
}

enumeration_result generic_map_foreach_const(const generic_map *map, const_generic_map_entry_callback callback, void *cookie)
{
    const_bucket_cb_args bucket_args = { .item_cb_args.item_cb = callback, .item_cb_args.item_cb_cookie = cookie };
    return generic_map_enum_buckets_const(map, const_generic_map_foreach_bucket_cb, &bucket_args);
}

static item_callback_result generic_map_visit_entry(void *cookie, size_t index, generic_map_entry_list *list)
{
    map_visitor *visitor = (map_visitor *) cookie;
    generic_map_entry *entry = generic_map_entry_list_payload(list);
    visitor->visit(visitor, entry);
    return CONTINUE_ENUMERATION;
}

typedef struct bucket_visit_args_struct {
    map_visitor *visitor;
} bucket_visit_args;

static item_callback_result generic_map_visit_bucket(void *cookie, generic_map_entry_list *bucket)
{
    bucket_visit_args *args = (bucket_visit_args *) cookie;
    enumeration_result enum_res = generic_map_entry_list_foreach(bucket, generic_map_visit_entry, args->visitor);
    if (enum_res == ENUMERATION_INTERRUPTED) {
        return STOP_ENUMERATION;
    }
    assert(ENUMERATION_COMPLETE == enum_res);
    return CONTINUE_ENUMERATION;
}

enumeration_result map_visitor_visit(map_visitor *visitor, generic_map *map)
{
    bucket_visit_args args = { .visitor = visitor };
    return generic_map_enum_buckets(map, generic_map_visit_bucket, &args);
}

static item_callback_result generic_map_visit_entry_const(void *cookie, size_t index, const generic_map_entry_list *list)
{
    const_map_visitor *visitor = (const_map_visitor *) cookie;
    const generic_map_entry *entry = generic_map_entry_list_payload_const(list);
    visitor->visit(visitor, entry);
    return CONTINUE_ENUMERATION;
}

typedef struct const_bucket_visit_args_struct {
    const_map_visitor *visitor;
} const_bucket_visit_args;

static item_callback_result generic_map_visit_bucket_const(void *cookie, const generic_map_entry_list *bucket)
{
    const_bucket_visit_args *args = (const_bucket_visit_args *) cookie;
    enumeration_result enum_res = generic_map_entry_list_foreach_const(bucket, generic_map_visit_entry_const, args->visitor);
    if (enum_res == ENUMERATION_INTERRUPTED) {
        return STOP_ENUMERATION;
    }
    assert(ENUMERATION_COMPLETE == enum_res);
    return CONTINUE_ENUMERATION;
}

enumeration_result const_map_visitor_visit(const_map_visitor *visitor, const generic_map *map)
{
    const_bucket_visit_args args = { .visitor = visitor };
    return generic_map_enum_buckets_const(map, generic_map_visit_bucket_const, &args);
}

typedef struct listentry_cb_args_struct {
    toy_str desired_name;
    generic_map_entry *entry_to_find;
} listentry_cb_args;

static item_callback_result generic_map_get_listentry_cb(void *cookie, size_t index, generic_map_entry_list *list)
{
    listentry_cb_args *args = (listentry_cb_args *) cookie;
    generic_map_entry *map_entry = generic_map_entry_list_payload(list);
    if (str_equal(map_entry->key, args->desired_name)) {
        args->entry_to_find = map_entry;
        return STOP_ENUMERATION;
    }
    return CONTINUE_ENUMERATION;
}

static generic_map_entry *generic_map_bucket_get_key(generic_map_entry_list *bucket, const toy_str key)
{
    listentry_cb_args args = { .desired_name = key, .entry_to_find = NULL };
    enumeration_result res = generic_map_entry_list_foreach(bucket, generic_map_get_listentry_cb, &args);
    assert(
        (res == ENUMERATION_COMPLETE && args.entry_to_find == NULL)
        ||
        (res = ENUMERATION_INTERRUPTED && args.entry_to_find != NULL)
    );
    return args.entry_to_find;
}

generic_map_entry *generic_map_get_entry(generic_map *map, const toy_str key)
{
    generic_map_entry_list *bucket = generic_map_get_bucket(map, key);
    if (bucket) {
        generic_map_entry *existing_entry = generic_map_bucket_get_key(bucket, key);
        if (existing_entry) {
            assert(str_equal(existing_entry->key, key));
            return existing_entry;
        }
        return NULL;
    }
    return NULL;
}

size_t generic_map_size(const generic_map *map)
{
    return map->num_items;
}

void generic_map_assert_valid(const generic_map *map)
{
    assert(map);
    /* TODO */
}

const generic_map_entry *generic_map_get_entry_const(const generic_map *map, const toy_str key)
{
    return (const generic_map_entry *) generic_map_get_entry((generic_map *) map, key);
}

static enumeration_result generic_map_filter_visitor_visit_entry(map_filter *visitor, generic_map_entry *entry)
{
    if (visitor->filter(visitor->filter_cookie, entry)) {
        assert(
            (void *) visitor->visit ==
            (void *) ((map_visitor *) visitor)->visit
        );
        return visitor->visit(visitor, entry);
    }
    return CONTINUE_ENUMERATION;
}

/* TODO: Should this take a generic_map_filter_visitor or a generic_map_visitor? */
enumeration_result generic_map_filter_visitor_visit_map(map_filter *visitor, generic_map *map)
{
    /* FIXME: Nevr uses visitor parameter */
    assert(0);
    map_visitor filter_visitor = {
        .visit = (map_entry_visit_func) generic_map_filter_visitor_visit_entry
    };
    return map_visitor_visit(&filter_visitor, map);
}

typedef struct generic_map_find_one_args_struct {
    generic_map_filter_func filter_func;
    void *filter_cookie;
    generic_map_entry *found_entry;
} generic_map_find_one_args;

static item_callback_result generic_map_find_first_cb(void *cookie, generic_map_entry *entry)
{
    generic_map_find_one_args *args = (generic_map_find_one_args *) cookie;
    if (args->filter_func(args->filter_cookie, entry)) {
        args->found_entry = entry;
        return STOP_ENUMERATION;
    }
    return CONTINUE_ENUMERATION;
}

generic_map_entry *generic_map_find(generic_map *map, generic_map_filter_func filter, void *cookie)
{
    generic_map_find_one_args find_one_args = { .filter_cookie = cookie, .filter_func = filter, .found_entry = NULL };
    enumeration_result res = generic_map_foreach(map, generic_map_find_first_cb, &find_one_args);
    assert(
        (res == ENUMERATION_COMPLETE && find_one_args.found_entry == NULL)
        ||
        (res == ENUMERATION_INTERRUPTED && find_one_args.found_entry != NULL)
    );
    return find_one_args.found_entry;
}

static item_callback_result generic_map_find_first_not_cb(void *cookie, generic_map_entry *entry)
{
    generic_map_find_one_args *args = (generic_map_find_one_args *) cookie;
    if (!args->filter_func(args->filter_cookie, entry)) {
        args->found_entry = entry;
        return STOP_ENUMERATION;
    }
    return CONTINUE_ENUMERATION;
}

generic_map_entry *generic_map_find_not(generic_map *map, generic_map_filter_func filter, void *cookie)
{
    generic_map_find_one_args find_one_args = { .filter_cookie = cookie, .filter_func = filter, .found_entry = NULL };
    enumeration_result res = generic_map_foreach(map, generic_map_find_first_not_cb, &find_one_args);
    assert(
        (res == ENUMERATION_COMPLETE && find_one_args.found_entry == NULL)
        ||
        (res == ENUMERATION_INTERRUPTED && find_one_args.found_entry != NULL)
    );
    return find_one_args.found_entry;
}

toy_bool generic_map_none_match(generic_map *map, generic_map_filter_func filter, void *cookie)
{
    generic_map_entry *entry = generic_map_find(map, filter, cookie);
    return entry == NULL;
}

toy_bool generic_map_all_match(generic_map *map, generic_map_filter_func filter, void *cookie)
{
    generic_map_entry *entry = generic_map_find_not(map, filter, cookie);
    return entry == NULL;
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

void generic_map_dump_keys(const generic_map *map)
{
    log_putc('[');
    dump_keys_visitor visitor = { .visit = (const_map_entry_visit_func) dump_keys_cb, .output_anything = TOY_FALSE };
    enumeration_result res = const_map_visitor_visit((const_map_visitor *) &visitor, map);
    assert(ENUMERATION_COMPLETE == res);
    if (visitor.output_anything) {
        log_putc(' ');
    }
    log_putc(']');
}
