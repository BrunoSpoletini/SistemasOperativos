#ifndef _HASH_TABLE
#define _HASH_TABLE

typedef struct {
    char* key;
    int value;
} Entry;

typedef struct {
    Entry* entries;
    int size;
} HashTable;
unsigned long hash(const char* key);
void initHashTable(HashTable* table);
void insert(HashTable* table, const char* key, int value);
int get(HashTable* table, const char* key);
void delete(HashTable* table, const char* key);
void printTable(HashTable* table);

#include "hash_table.c"
#endif
