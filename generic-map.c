#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "str.h"
#include "generic-map.h"
#include "mymalloc.h"
#include "dump.h"
#include "errors.h"
#include "buf-list.h"

generic_map *generic_map_alloc(void)
{
    generic_map *map = mymalloc(generic_map);
    memset(map->buckets, 0, sizeof(map->buckets));
    map->num_items = 0;
    return map;
}

static void free_buckets(generic_map *map)
{
    /* TODO: Use enum buckets */
    for (toy_buf_list **bucket = &map->buckets[0]; bucket < &map->buckets[NUM_BUCKETS]; bucket++) {
        if (*bucket) {
            buf_list_free(*bucket);
            *bucket = 0;
        }
    }
}

void generic_map_reset(generic_map *map)
{
    free_buckets(map);
    map->num_items = 0;
}

void generic_map_free(generic_map *map)
{
    free_buckets(map);
    free(map);
}

static uint32_t jenkins_one_at_a_time_hash(const uint8_t* key, size_t length) {
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

toy_buf_list **generic_map_get_bucket(generic_map *map, toy_str key)
{
    uint32_t hashval = jenkins_one_at_a_time_hash((uint8_t *) key, strlen(key));
    return &map->buckets[hashval % NUM_BUCKETS];
}

int generic_map_delete(generic_map *map, const toy_str key)
{
    toy_buf_list **bucket = generic_map_get_bucket(map, key);
    if (*bucket) {
        toy_buf_list *list, *prev;
        for (list = *bucket, prev = *bucket; list; prev = list, list = list->next) {
            generic_map_entry *map_entry = buf_list_payload(list);
            if (toy_str_equal(map_entry->key, key)) {
                /* Found existing entry */
                prev->next = list->next;
                if (list == *bucket) {
                    assert(list->next == NULL);
                    *bucket = NULL;
                } else {
                    list->next = NULL;
                }
                buf_list_free(list);
                map->num_items--;
                return 1;
            }
        }
        return 0; /* No entry */
    }
    return 0; /* No bucket, so no entry */
}

static void dump_map_entry(FILE *f, const generic_map_entry *entry)
{
    dump_str(f, entry->key);
    fprintf(f, ": %p", &entry->payload);
}

void generic_map_dump(FILE *f, const generic_map *map)
{
    int output_anything = 0;

    fputc('{', f);
    for (toy_buf_list * const * bucket = &map->buckets[0]; bucket < &map->buckets[NUM_BUCKETS]; bucket++) {
        if (*bucket) {
            for (toy_buf_list *entry = *bucket; entry; entry = entry->next) {
                if (output_anything) {
                    fputs(", ", f);
                } else {
                    fputc(' ', f);
                }
                assert(offsetof(generic_map_entry_list, entry) == offsetof(toy_buf_list, c));
                generic_map_entry *map_entry = (generic_map_entry *) &entry->c;
                dump_map_entry(f, map_entry);
                output_anything = 1;
            }
        }
    }
    if (output_anything) {
        fputc(' ', f);
    }
    fputc('}', f);
}

void generic_map_dump_keys(FILE *f, const generic_map *map)
{
    int output_anything = 0;

    fputc('[', f);
    for (toy_buf_list * const * bucket = &map->buckets[0]; bucket < &map->buckets[NUM_BUCKETS]; bucket++) {
        if (*bucket) {
            for (toy_buf_list *entry = *bucket; entry; entry = entry->next) {
                if (output_anything) {
                    fputs(", ", f);
                } else {
                    fputc(' ', f);
                }
                assert(offsetof(generic_map_entry_list, entry) == offsetof(toy_buf_list, c));
                generic_map_entry *map_entry = (generic_map_entry *) &entry->c;
                dump_str(f, map_entry->key);
                output_anything = 1;
            }
        }
    }
    if (output_anything) {
        fputc(' ', f);
    }
    fputc(']', f);
}

typedef item_callback_result (*generic_map_bucket_callback)(void *cookie, toy_buf_list *bucket);

static enumeration_result generic_map_enum_buckets(generic_map *map, generic_map_bucket_callback callback, void *cookie)
{
    for (toy_buf_list * const * bucket = &map->buckets[0]; bucket < &map->buckets[NUM_BUCKETS]; bucket++) {
        if (*bucket) {
            item_callback_result res = callback(cookie, *bucket);
            if (res == STOP_ENUMERATION) {
                return ENUMERATION_INTERRUPTED;
            }
        }
    }
    return ENUMERATION_COMPLETE;
}

typedef struct buflist_item_cb_args_struct {
    generic_map_entry_callback item_cb;
    void *item_cb_cookie;
} buflist_item_cb_args;

static item_callback_result buflist_item_cb(void *cookie, size_t index, toy_buf_list *list)
{
    buflist_item_cb_args *args = (buflist_item_cb_args *) cookie;
    generic_map_entry *entry = buf_list_payload_typed(list, generic_map_entry);
    return args->item_cb(args->item_cb_cookie, entry->key, &entry->payload);
}

typedef struct bucket_cb_args_struct {
    buflist_item_cb_args item_cb_args;
} bucket_cb_args;

static item_callback_result generic_map_foreach_bucket_cb(void *cookie, toy_buf_list *bucket)
{
    bucket_cb_args *args = (bucket_cb_args *) cookie;
    enumeration_result enum_res = buf_list_foreach(bucket, buflist_item_cb, &args->item_cb_args);
    if (enum_res == ENUMERATION_INTERRUPTED) {
        return STOP_ENUMERATION;
    }
    assert(ENUMERATION_COMPLETE == enum_res);
    return CONTINUE_ENUMERATION;
}

static generic_map_entry *generic_map_entry_list_payload(toy_buf_list *list)
{
    return (generic_map_entry *) buf_list_payload(list);
}

enumeration_result generic_map_foreach(generic_map *map, generic_map_entry_callback callback, void *cookie)
{
    bucket_cb_args bucket_args = { .item_cb_args.item_cb = callback, .item_cb_args.item_cb_cookie = cookie };
    return generic_map_enum_buckets(map, generic_map_foreach_bucket_cb, &bucket_args);
}

enumeration_result generic_map_foreach_const(const generic_map *map, const_generic_map_entry_callback callback, void *cookie)
{
    return generic_map_foreach((generic_map *) map, (generic_map_entry_callback) callback, cookie);
}

typedef struct listentry_cb_args_struct {
    toy_str desired_name;
    generic_map_entry *entry_to_find;
} listentry_cb_args;

static item_callback_result generic_map_get_listentry_cb(void *cookie, size_t index, toy_buf_list *list)
{
    listentry_cb_args *args = (listentry_cb_args *) cookie;
    generic_map_entry *map_entry = generic_map_entry_list_payload(list);
    if (toy_str_equal(map_entry->key, args->desired_name)) {
        args->entry_to_find = map_entry;
        return STOP_ENUMERATION;
    }
    return CONTINUE_ENUMERATION;
}

generic_map_entry *generic_map_bucket_get_key(toy_buf_list *bucket, const toy_str key)
{
    listentry_cb_args args = { .desired_name = key, .entry_to_find = NULL };
    enumeration_result res = buf_list_foreach(bucket, generic_map_get_listentry_cb, &args);
    assert(
        (res == ENUMERATION_COMPLETE && args.entry_to_find == NULL)
        ||
        (res = ENUMERATION_INTERRUPTED && args.entry_to_find != NULL)
    );
    return args.entry_to_find;
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
