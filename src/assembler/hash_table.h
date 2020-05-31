#ifndef HASHTABLE_H
#define HASHTABLE_H
#include <stdint.h>
#include <stdlib.h>

#define TABLE_SIZE 20000
typedef void *hashkey;
typedef void *hashvalue;

typedef struct {
    hashkey key;
    hashvalue value;
    size_t key_size; 
    struct Entry *next;
} Entry;

typedef struct {
    int (*comp)(const void *, const void *);
    Entry **entries;
} HashTable;


// Hash function 
unsigned long hash(const hashkey key, size_t size); 

// Creates a HashTable
HashTable *ht_create(int (*comp)(const void*, const void*)); 

// sets the value of given key
void ht_set(HashTable *hashtable, const hashkey key, const hashvalue value, size_t size);

// Gets the pointer to the value or NULL if failed
hashvalue ht_get(HashTable *hashtable, const hashkey key, size_t key_size);

// Deletes a key from the hashtable
void ht_del(HashTable *hashtable, const hashkey key, size_t key_size);

// Free hashtable
void ht_free(HashTable *hashtable);

#endif
