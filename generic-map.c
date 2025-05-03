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

/* TODO: dynamic resizing */
#define NUM_BUCKETS 13

/* TODO: Use generic list lib */
typedef struct generic_map_entry_struct {
    toy_str key;
    toy_val value;
} generic_map_entry;

typedef struct ptr_map_entry_struct {
    toy_str key;
    void *ptr;
} ptr_map_entry;
typedef struct ptr_map_entry_list_struct {
    struct ptr_map_entry_list_struct *next;
    ptr_map_entry entry;
} ptr_map_entry_list;

typedef struct buf_map_entry_struct {
    toy_str key;
    uint8_t c; /* Really varable-size */
} buf_map_entry;
typedef struct buf_map_entry_list_struct {
    struct buf_map_entry_list_struct *next;
    buf_map_entry entry;
} buf_map_entry_list;

typedef struct generic_map_entry_list_struct {
    struct generic_map_entry_list_struct *next;
    generic_map_entry entry;
} generic_map_entry_list;

struct toy_map_struct {
    size_t num_items;
    generic_map_entry_list *buckets[NUM_BUCKETS];
};

toy_map *generic_map_alloc(void)
{
    toy_map *map = mymalloc(toy_map);
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

static void free_buckets(toy_map *map)
{
    for (generic_map_entry_list **bucket = &map->buckets[0]; bucket < &map->buckets[NUM_BUCKETS]; bucket++) {
        if (*bucket) {
            free_bucket_entries(*bucket);
            *bucket = 0;
        }
    }
}

void generic_map_reset(toy_map *map)
{
    free_buckets(map);
    map->num_items = 0;
}

void generic_map_free(toy_map *map)
{
    free_buckets(map);
    free(map);
}

static generic_map_entry_list *alloc_map_entry(toy_str key_name, toy_val *value)
{
    generic_map_entry_list *entry;
    entry = mymalloc(generic_map_entry_list);
    entry->entry.key = key_name;
    entry->entry.value = *value;
    entry->next = NULL;
    return entry;
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

static generic_map_entry_list **get_bucket(toy_map *map, toy_str key)
{
    uint32_t hashval = jenkins_one_at_a_time_hash((uint8_t *) key, strlen(key));
    return &map->buckets[hashval % NUM_BUCKETS];
}

static toy_val *get_bucket_key(generic_map_entry_list *bucket, const toy_str key)
{
    for (generic_map_entry_list *entry = bucket; entry; entry = entry->next) {
        if (toy_str_equal(entry->entry.key, key)) {
            return &entry->entry.value;
        }
    }
    return NULL;
}

toy_val *map_val_get(toy_map *map, const toy_str key)
{
    generic_map_entry_list **bucket = get_bucket(map, key);
    if (*bucket) {
        toy_val *existing_value = get_bucket_key(*bucket, key);
        if (existing_value) {
            return existing_value;
        }
        return NULL;
    }
    return NULL;
}

int map_val_set(toy_map *map, const toy_str key, const toy_val *value)
{
    generic_map_entry_list *new_entry;
    generic_map_entry_list **bucket = get_bucket(map, key);
    if (*bucket) {
        for (generic_map_entry_list *entry = *bucket; entry; entry = entry->next) {
            if (toy_str_equal(entry->entry.key, key)) {
                /* Overwrite existing entry */
                memcpy(&entry->entry.value, value, sizeof(*value));
                return 0;
            }
        }
        /* Prepend new entry to existing bucket */
        new_entry = alloc_map_entry(key, (toy_val *) value);
        new_entry->next = *bucket;
    } else {
        /* New entry in new bucket */
        new_entry = alloc_map_entry(key, (toy_val *) value);
    }
    *bucket = new_entry;
    map->num_items++;
    return 1;
}

int generic_map_delete(toy_map *map, const toy_str key)
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

void generic_map_dump(FILE *f, const toy_map *map)
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

void generic_map_dump_keys(FILE *f, const toy_map *map)
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

enumeration_result generic_map_foreach(toy_map *map, map_entry_callback callback, void *cookie)
{
    for (generic_map_entry_list * const * bucket = &map->buckets[0]; bucket < &map->buckets[NUM_BUCKETS]; bucket++) {
        if (*bucket) {
            for (generic_map_entry_list *entry = *bucket; entry; entry = entry->next) {
                item_callback_result res = callback(cookie, entry->entry.key, &entry->entry.value);
                if (res == STOP_ENUMERATION) {
                    return EUMERATION_INTERRUPTED;
                }
            }
        }
    }
    return ENUMERATION_COMPLETE;
}

enumeration_result generic_map_foreach_const(const toy_map *map, const_map_entry_callback callback, void *cookie)
{
    return generic_map_foreach((toy_map *) map, (map_entry_callback) callback, cookie);
}

size_t map_len(const toy_map *map)
{
    return map->num_items;
}

void generic_map_assert_valid(const toy_map *map)
{
    assert(map);
    /* TODO */
}
