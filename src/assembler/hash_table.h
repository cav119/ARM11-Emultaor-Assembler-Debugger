#ifndef HASHTABLE_H
#define HASHTABLE_H
#include <stdint.h>

#define TABLE_SIZE 20000

typedef struct {
    char *key;
    char *value;
    struct Entry *next;
} Entry;

typedef struct {
    Entry **entries;
} HashTable;


// Hash function 
unsigned int hash(const char *key);

// Creates a HashTable
HashTable *ht_create(void); 

// sets the value of given key
void ht_set(HashTable *hashtable, const char *key, const char *value); 

// Gets the pointer to the value or NULL if failed
char *ht_get(HashTable *hashtable, const char *key);


// Deletes the key from the table
void ht_del(HashTable *hashtable, const char *key); 

// Prints contents to stdout
void ht_dump(HashTable *hashtable); 

// Test a (string, string) hashtable
void test_ht(void);


#endif
