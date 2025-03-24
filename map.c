#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "map.h"
#include "mymalloc.h"

toy_map *alloc_map()
{
    toy_map *map;
    map = (toy_map *) malloc(sizeof(toy_map));
    memset(map->buckets, 0, sizeof(map->buckets));
    map->num_items = 0;
    return map;
}

static toy_map_entry *alloc_map_entry_str(toy_str key, toy_expr *value)
{
    toy_map_entry *map_entry;
    map_entry = (toy_map_entry *) malloc(sizeof(toy_map_entry) + sizeof(toy_expr));
    map_entry->key = (toy_expr *) (map_entry + 1);
    map_entry->key->type = EXPR_STR;
    map_entry->key->str = key;
    map_entry->value = value;
    map_entry->next = NULL;
    return map_entry;
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

static toy_map_entry **get_bucket(toy_map *map, toy_str key)
{
    uint32_t hashval = jenkins_one_at_a_time_hash((uint8_t *) key, strlen(key));
    return &map->buckets[hashval % NUM_BUCKETS];
}

static toy_expr **get_bucket_key(toy_map_entry *bucket, const toy_str key)
{
    for (toy_map_entry *entry = bucket; entry; entry = entry->next) {
        assert(EXPR_STR == entry->key->type);
        if (0 == strcmp(entry->key->str, key)) {
            return &entry->value;
        }
    }
    return NULL;
}

toy_expr *map_get(toy_map *map, const toy_str key)
{
    toy_map_entry **bucket = get_bucket(map, key);
    if (*bucket) {
        toy_expr **existing_value = get_bucket_key(*bucket, key);
        if (existing_value) {
            return *existing_value;
        }
        return NULL;
    }
    return NULL;
}

void map_set(toy_map *map, const toy_str key, toy_expr *value)
{
    toy_map_entry **bucket = get_bucket(map, key);
    if (*bucket) {
        toy_map_entry *entry;
        for (entry = *bucket; entry->next; entry = entry->next) {
            assert(EXPR_STR == entry->key->type);
            if (0 == strcmp(entry->key->str, key)) {
                entry->value = value;
                return;
            }
        }
        assert(EXPR_STR == entry->key->type);
        if (0 == strcmp(entry->key->str, key)) {
            entry->value = value;
            return;
        }
        toy_map_entry *new_entry = alloc_map_entry_str(key, value);
        entry->next = new_entry;
        map->num_items++;
    } else {
        toy_map_entry *new_entry = alloc_map_entry_str(key, value);
        *bucket = new_entry;
        map->num_items++;
    }
}

void map_set_expr(toy_map *map, const toy_expr *key, toy_expr *value)
{
    if (EXPR_STR == key->type) {
        map_set(map, key->str, value);
    } else {
        invalid_operand(EXPR_LIST, key);
    }
}

void map_delete(toy_map *map, const toy_str key)
{
    /* TODO */
    map->num_items--;
}

void dump_map(FILE *f, const toy_map *map)
{
    int output_anything = 0;

    fputc('{', f);
    for (toy_map_entry * const * bucket = &map->buckets[0]; bucket < &map->buckets[NUM_BUCKETS]; bucket++) {
        if (*bucket) {
            for (toy_map_entry *entry = *bucket; entry; entry = entry->next) {
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

void map_enum(const toy_map *map, map_entry_callback callback, void *cookie)
{
    for (toy_map_entry * const * bucket = &map->buckets[0]; bucket < &map->buckets[NUM_BUCKETS]; bucket++) {
        if (*bucket) {
            for (toy_map_entry *entry = *bucket; entry; entry = entry->next) {
                assert(EXPR_STR == entry->key->type);
                callback(cookie, entry->key->str, entry->value);
            }
        }
    }
}

void dump_map_entry(FILE *f, const toy_map_entry *entry)
{
    dump_expr(f, entry->key);
    fputs(": ", f);
    dump_expr(f, entry->value);
}

void dump_map_entries(FILE *f, const toy_map_entry *entries)
{
    for (; entries; entries = entries->next) {
        dump_map_entry(f, entries);
    }
}
