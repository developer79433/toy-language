#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "str.h"
#include "generic-map.h"
#include "val.h"
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

static void free_bucket_entries(generic_map_entry_list *entry)
{
    while (entry) {
        generic_map_entry_list *next = entry->next;
        /* We don't own this storage */
#if 0
        toy_str_free(entry->key);
        val_free(&entry->value);
#endif
        free(entry);
        entry = next;
    }
}

static void free_buckets(generic_map *map)
{
    for (generic_map_entry_list **bucket = &map->buckets[0]; bucket < &map->buckets[NUM_BUCKETS]; bucket++) {
        if (*bucket) {
            free_bucket_entries(*bucket);
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

generic_map_entry_list *map_val_entry_list_alloc(toy_str key_name, toy_val *value)
{
    generic_map_entry_list *entry_list;
    entry_list = mymalloc(generic_map_entry_list);
    entry_list->entry.key = key_name;
    entry_list->entry.value = *value;
    entry_list->next = NULL;
    return entry_list;
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

generic_map_entry_list **get_bucket(generic_map *map, toy_str key)
{
    uint32_t hashval = jenkins_one_at_a_time_hash((uint8_t *) key, strlen(key));
    return &map->buckets[hashval % NUM_BUCKETS];
}

int generic_map_delete(generic_map *map, const toy_str key)
{
    generic_map_entry_list **bucket = get_bucket(map, key);
    if (*bucket) {
        generic_map_entry_list *entry, *prev;
        for (entry = *bucket, prev = *bucket; entry; prev = entry, entry = entry->next) {
            if (toy_str_equal(entry->entry.key, key)) {
                /* Found existing entry */
                prev->next = entry->next;
                if (entry == *bucket) {
                    assert(entry->next == NULL);
                    *bucket = NULL;
                }
                free(entry);
                map->num_items--;
                return 1;
            }
        }
        return 0; /* No entry */
    }
    return 0; /* No bucket, so no entry */
}

static void dump_map_entry(FILE *f, const generic_map_entry_list *entry)
{
    dump_str(f, entry->entry.key);
    fputs(": ", f);
    val_dump(f, &entry->entry.value);
}

void generic_map_dump(FILE *f, const generic_map *map)
{
    int output_anything = 0;

    fputc('{', f);
    for (generic_map_entry_list * const * bucket = &map->buckets[0]; bucket < &map->buckets[NUM_BUCKETS]; bucket++) {
        if (*bucket) {
            for (generic_map_entry_list *entry = *bucket; entry; entry = entry->next) {
                if (output_anything) {
                    fputs(", ", f);
                } else {
                    fputc(' ', f);
                }
                dump_map_entry(f, entry);
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
    for (generic_map_entry_list * const * bucket = &map->buckets[0]; bucket < &map->buckets[NUM_BUCKETS]; bucket++) {
        if (*bucket) {
            for (generic_map_entry_list *entry = *bucket; entry; entry = entry->next) {
                if (output_anything) {
                    fputs(", ", f);
                } else {
                    fputc(' ', f);
                }
                dump_str(f, entry->entry.key);
                output_anything = 1;
            }
        }
    }
    if (output_anything) {
        fputc(' ', f);
    }
    fputc(']', f);
}

typedef item_callback_result (*generic_map_bucket_callback)(void *cookie, generic_map_entry_list *list);

static enumeration_result generic_map_enum_buckets(generic_map *map, generic_map_bucket_callback callback, void *cookie)
{
    for (generic_map_entry_list * const * bucket = &map->buckets[0]; bucket < &map->buckets[NUM_BUCKETS]; bucket++) {
        if (*bucket) {
            item_callback_result res = callback(cookie, *bucket);
            if (res == STOP_ENUMERATION) {
                return ENUMERATION_INTERRUPTED;
            }
        }
    }
    return ENUMERATION_COMPLETE;
}

typedef struct foreach_bucket_cb_args_struct {
    generic_map_entry_callback entry_cb;
    void *entry_cb_cookie;
} foreach_bucket_cb_args;

static item_callback_result generic_map_foreach_bucket_cb(void *cookie, generic_map_entry_list *list)
{
    foreach_bucket_cb_args *args = (foreach_bucket_cb_args *) cookie;
    for (generic_map_entry_list *entry = list; entry; ) {
        generic_map_entry_list *next = entry->next;
        item_callback_result res = args->entry_cb(args->entry_cb_cookie, entry->entry.key, &entry->entry.value);
        if (res == STOP_ENUMERATION) {
            return res;
        }
        entry = next;
    }
    return CONTINUE_ENUMERATION;
}

enumeration_result generic_map_foreach(generic_map *map, generic_map_entry_callback callback, void *cookie) {
    foreach_bucket_cb_args args = { .entry_cb = callback, .entry_cb_cookie = cookie };
    return generic_map_enum_buckets(map, generic_map_foreach_bucket_cb, &args);
}

enumeration_result generic_map_foreach_const(const generic_map *map, const_generic_map_entry_callback callback, void *cookie)
{
    return generic_map_foreach((generic_map *) map, (generic_map_entry_callback) callback, cookie);
}

enumeration_result generic_map_bucket_enum_entries(generic_map_entry_list *bucket, generic_map_bucket_callback callback, void *cookie)
{
    for (generic_map_entry_list *entry = bucket; entry; ) {
        generic_map_entry_list *next = entry->next;
        item_callback_result res = callback(cookie, entry);
        if (res == STOP_ENUMERATION) {
            return ENUMERATION_INTERRUPTED;
        }
        entry = next;
    }
    return ENUMERATION_COMPLETE;
}

typedef struct get_entry_cb_args_struct {
    toy_str desired_name;
    toy_val *val_to_set;
} get_entry_cb_args;

static item_callback_result generic_map_get_bucket_cb(void *cookie, generic_map_entry_list *list)
{
    foreach_bucket_cb_args *args = (foreach_bucket_cb_args *) cookie;
    for (generic_map_entry_list *entry = list; entry; ) {
        generic_map_entry_list *next = entry->next;
        item_callback_result res = args->entry_cb(args->entry_cb_cookie, entry->entry.key, &entry->entry.value);
        if (res == STOP_ENUMERATION) {
            return res;
        }
        entry = next;
    }
    return CONTINUE_ENUMERATION;
}

static item_callback_result generic_map_get_entry_cb(void *cookie, toy_str key, toy_val *value)
{
    get_entry_cb_args *args = (get_entry_cb_args *) cookie;
    if (toy_str_equal(key, args->desired_name)) {
        args->val_to_set = value;
        return STOP_ENUMERATION;
    }
    return CONTINUE_ENUMERATION;
}

toy_val *map_val_get_bucket_key(generic_map_entry_list *bucket, const toy_str key)
{
    get_entry_cb_args entry_cb_args = { .desired_name = key, .val_to_set = NULL };
    foreach_bucket_cb_args args = { .entry_cb = generic_map_get_entry_cb, .entry_cb_cookie = &entry_cb_args };
    enumeration_result res = generic_map_bucket_enum_entries(bucket, generic_map_get_bucket_cb, &args);
    assert(
        (res == ENUMERATION_COMPLETE && entry_cb_args.val_to_set == NULL)
        ||
        (res = ENUMERATION_INTERRUPTED && entry_cb_args.val_to_set != NULL)
    );
    return entry_cb_args.val_to_set;
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
