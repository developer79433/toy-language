#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "str.h"
#include "map.h"
#include "val.h"
#include "mymalloc.h"
#include "dump.h"
#include "errors.h"

/* TODO: dynamic resizing */
#define NUM_BUCKETS 13

/* TODO: Use generic list lib */
typedef struct map_entry_struct {
    struct map_entry_struct *next;
    toy_str key;
    toy_val value;
} map_entry;

struct toy_map_struct {
    size_t num_items;
    map_entry *buckets[NUM_BUCKETS];
};

toy_map *map_alloc(void)
{
    toy_map *map = mymalloc(toy_map);
    memset(map->buckets, 0, sizeof(map->buckets));
    map->num_items = 0;
    return map;
}

static void free_bucket_entries(map_entry *entry)
{
    while (entry) {
        map_entry *next = entry->next;
        free(entry);
        entry = next;
    }
}

static void free_buckets(toy_map *map)
{
    for (map_entry **bucket = &map->buckets[0]; bucket < &map->buckets[NUM_BUCKETS]; bucket++) {
        if (*bucket) {
            free_bucket_entries(*bucket);
            *bucket = 0;
        }
    }
}

void map_reset(toy_map *map)
{
    free_buckets(map);
    map->num_items = 0;
}

void map_free(toy_map *map)
{
    free_buckets(map);
    free(map);
}

static map_entry *alloc_map_entry(toy_str key_name, toy_val *value)
{
    map_entry *entry;
    entry = mymalloc(map_entry);
    entry->key = key_name;
    entry->value = *value;
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

static map_entry **get_bucket(toy_map *map, toy_str key)
{
    uint32_t hashval = jenkins_one_at_a_time_hash((uint8_t *) key, strlen(key));
    return &map->buckets[hashval % NUM_BUCKETS];
}

static toy_val *get_bucket_key(map_entry *bucket, const toy_str key)
{
    for (map_entry *entry = bucket; entry; entry = entry->next) {
        if (toy_str_equal(entry->key, key)) {
            return &entry->value;
        }
    }
    return NULL;
}

toy_val *map_get(toy_map *map, const toy_str key)
{
    map_entry **bucket = get_bucket(map, key);
    if (*bucket) {
        toy_val *existing_value = get_bucket_key(*bucket, key);
        if (existing_value) {
            return existing_value;
        }
        return NULL;
    }
    return NULL;
}

int map_set(toy_map *map, const toy_str key, const toy_val *value)
{
    map_entry *new_entry;
    map_entry **bucket = get_bucket(map, key);
    if (*bucket) {
        for (map_entry *entry = *bucket; entry; entry = entry->next) {
            if (toy_str_equal(entry->key, key)) {
                /* Overwrite existing entry */
                memcpy(&entry->value, value, sizeof(*value));
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

int map_delete(toy_map *map, const toy_str key)
{
    map_entry **bucket = get_bucket(map, key);
    if (*bucket) {
        map_entry *entry, *prev;
        for (entry = *bucket, prev = *bucket; entry; prev = entry, entry = entry->next) {
            if (toy_str_equal(entry->key, key)) {
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

static void dump_map_entry(FILE *f, const map_entry *entry)
{
    dump_str(f, entry->key);
    fputs(": ", f);
    dump_val(f, &entry->value);
}

void map_dump(FILE *f, const toy_map *map)
{
    int output_anything = 0;

    fputc('{', f);
    for (map_entry * const * bucket = &map->buckets[0]; bucket < &map->buckets[NUM_BUCKETS]; bucket++) {
        if (*bucket) {
            for (map_entry *entry = *bucket; entry; entry = entry->next) {
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

void map_dump_keys(FILE *f, const toy_map *map)
{
    int output_anything = 0;

    fputc('[', f);
    for (map_entry * const * bucket = &map->buckets[0]; bucket < &map->buckets[NUM_BUCKETS]; bucket++) {
        if (*bucket) {
            for (map_entry *entry = *bucket; entry; entry = entry->next) {
                if (output_anything) {
                    fputs(", ", f);
                } else {
                    fputc(' ', f);
                }
                dump_str(f, entry->key);
                output_anything = 1;
            }
        }
    }
    if (output_anything) {
        fputc(' ', f);
    }
    fputc(']', f);
}

void map_foreach(toy_map *map, map_entry_callback callback, void *cookie)
{
    for (map_entry * const * bucket = &map->buckets[0]; bucket < &map->buckets[NUM_BUCKETS]; bucket++) {
        if (*bucket) {
            for (map_entry *entry = *bucket; entry; entry = entry->next) {
                callback(cookie, entry->key, &entry->value);
            }
        }
    }
}

void map_foreach_const(const toy_map *map, const_map_entry_callback callback, void *cookie)
{
    map_foreach((toy_map *) map, (map_entry_callback) callback, cookie);
}

size_t map_len(const toy_map *map)
{
    return map->num_items;
}

void map_assert_valid(const toy_map *map)
{
    assert(map);
    /* TODO */
}
