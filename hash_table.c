#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "hash_table.h"

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL
#define DEFAULT_CAPACITY 32;

table *ht_create()
{
    table *ht = malloc(sizeof(table));
    if (ht == NULL) {
        fprintf(stderr, "Error while allocating memory for table!\n");
        return NULL;
    }
    ht->capacity = DEFAULT_CAPACITY;
    ht->size = 0;
    ht->entries = calloc(ht->capacity, sizeof(entry));
    if (ht->entries == NULL) {
        fprintf(stderr, "Error while allocating memory for entries!\n");
        free(ht);
        return NULL;
    }
    return ht;
}

void ht_destroy(table *ht)
{
    free(ht->entries);
    free(ht);
}

static uint64_t hash_key(char *key)
{
    uint64_t hash = FNV_OFFSET;
    for (char *p = key; *p; p++) {
        hash ^= (uint64_t) (unsigned char) (*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

void *ht_get(table *ht, char *key)
{
    // AND hash with capacity-1 to ensure its within entries array
    uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(ht->capacity - 1));

    // loop until we find empty entry
    while (ht->entries[index].key != NULL) {
        if (strcmp(key, ht->entries[index].key) == 0) {
            return ht->entries[index].value;
        }
        index++;
        if (index >= ht->capacity) {
            index = 0;
        }
    }
    return NULL;
}

static char *ht_set_entry(entry *entries, size_t capacity,
         char *key, void *value, size_t *p_length) {
    // AND hash with capacity-1 to ensure its within entries array
    uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(capacity - 1));

    // loop until we find empty entry
    while (entries[index].key != NULL) {
        if (strcmp(key, entries[index].key) == 0) {
            entries[index].value = value;
            return entries[index].key;
        }
        index++;
        if (index >= capacity) {
            index = 0;
        }
    }

    if (p_length != NULL) {
        if (key == NULL) {
            return NULL;
        }
        (*p_length)++;
    }
    entries[index].key = key;
    entries[index].value = value;
    return key;
}

static bool ht_resize(table *ht)
{
    size_t new_capacity = ht->capacity * 2;
    if (new_capacity < ht->capacity) {
        fprintf(stderr, "Error while resizing table!\n");
        return false;
    }

    entry *new_entries = calloc(new_capacity, sizeof(entry));
    if (new_entries == NULL) {
        fprintf(stderr, "Error while allocating memory for entries!\n");
        return false;
    }

    for (size_t i = 0; i < ht->capacity; i++) {
        entry entry = ht->entries[i];
        if (entry.key != NULL) {
            ht_set_entry(new_entries, new_capacity, entry.key, entry.value, NULL);
        }
    }

    free(ht->entries);
    ht->entries = new_entries;
    ht->capacity = new_capacity;
    return true;
}

char *ht_set(table *ht, char *key, void *value)
{
    if (value == NULL) {
        return NULL;
    }

    // if size will exceed half of capacity, resize it
    if (ht->size >= ht->capacity / 2) {
        if (!ht_resize(ht)) {
            return NULL;
        }
    }
    return ht_set_entry(ht->entries, ht->capacity, key, value, &ht->size);
}

size_t ht_size(table *ht)
{
    return ht->size;
}

int main(void)
{
    table *ht = ht_create();
    int x = 53434;
    ht_set(ht, "idk", "neviem");
    ht_set(ht, "blabla", &x);
    printf("%s\n", (char *) ht_get(ht, "idk"));
    printf("%d\n", *(int *) ht_get(ht, "blabla"));
    ht_destroy(ht);
    return EXIT_SUCCESS;
}
