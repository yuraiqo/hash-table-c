#ifndef HASH_TABLE_H
#define HASH_TABLE_H

typedef struct {
    char *key;
    void *value;
} entry;

typedef struct {
    size_t capacity;
    size_t size;
    entry *entries;
} table;

table *create_table();

void delete_table(table *ht);

#endif