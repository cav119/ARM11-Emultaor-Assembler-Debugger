#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_table.h"
 
unsigned long hash(const hashkey key, size_t size) {
    char *char_key = (char *) key;
    unsigned long value = 0;
    unsigned int i = 0;

    // do several rounds of multiplication
    for (; i < size; i++) {
        value = value * 37 + char_key[i];
    }

    // make sure value is 0 <= value < TABLE_SIZE
    value = value % TABLE_SIZE;

    return value;
} 

Entry *ht_pair(const hashkey key, const hashkey value, size_t key_size) {
    // allocate the entry
    Entry *entry = malloc(sizeof(Entry));
    if (!entry) {
        perror("Error allocating memory for hash table entry");
        exit(EXIT_FAILURE);
    }
    entry->key = key;
    entry->value = value;
    entry->key_size = key_size;

    // next starts out null but may be set later on
    entry->next = NULL;

    return entry;
}

HashTable *ht_create(int (*comp)(const void*, const void*)) {
    // allocate table
    HashTable *hashtable = malloc(sizeof(HashTable));
    if (!hashtable) {
        perror("Error allocating memory for hash table");
        exit(EXIT_FAILURE);
    }
    hashtable->comp = comp;
    // allocate table entries
    hashtable->entries = malloc(sizeof(Entry*) * TABLE_SIZE);
    if (!hashtable->entries) {
        perror("Error allocating memory for hash table's entries");
        exit(EXIT_FAILURE);
    }

    // set each to null (needed for proper operation)
    int i = 0;
    for (; i < TABLE_SIZE; ++i) {
        hashtable->entries[i] = NULL;
    }

    return hashtable;
}

void ht_set(HashTable *hashtable, const hashkey key, const hashvalue value, size_t size) {
    unsigned long slot = hash(key, size);

    // try to look up an entry set
    Entry *entry = hashtable->entries[slot];

    // no entry means slot empty, insert immediately
    if (entry == NULL) {
        hashtable->entries[slot] = ht_pair(key, value, size);
        return;
    }

    Entry *prev;

    // walk through each entry until either the end is
    // reached or a matching key is found
    while (entry != NULL) {
        // check key
        if (hashtable->comp(entry->key, key)) {
            // match found, replace value
            free(entry->value);
            entry->value = value;
            return;
        }

        // walk to next
        prev = entry;
        entry = (Entry *) prev->next;
    }

    // end of chain reached without a match, add new
    prev->next = (struct Entry *) ht_pair(key, value, size);
}

hashvalue ht_get(HashTable *hashtable, const hashkey key, size_t key_size) {
    unsigned int slot = hash(key, key_size);

    // try to find a valid slot
    Entry *entry = hashtable->entries[slot];

    // no slot means no entry
    if (entry == NULL) {
        return NULL;
    }

    // walk through each entry in the slot, which could just be a single thing
    while (entry != NULL) {
        // return value if found
        if (hashtable->comp(entry->key, key)) {
            return entry->value;
        }

        // proceed to next key if available
        entry = (Entry *) entry->next;
    }

    // reaching here means there were >= 1 entries but no key match
    return NULL;
}

void ht_del(HashTable *hashtable, const hashkey key, size_t key_size) {
    unsigned int bucket = hash(key, key_size);

    // try to find a valid bucket
    Entry *entry = hashtable->entries[bucket];

    // no bucket means no entry
    if (entry == NULL) {
        return;
    }

    Entry *prev;
    int idx = 0;

    // walk through each entry until either the end is reached or a matching key is found
    while (entry != NULL) {
        // check key
        if (hashtable->comp(entry->key, key)) {
            // first item and no next entry
            if (entry->next == NULL && idx == 0) {
                hashtable->entries[bucket] = NULL;
            }

            // first item with a next entry
            if (entry->next != NULL && idx == 0) {
                hashtable->entries[bucket] = (Entry *) entry->next;
            }

            // last item
            if (entry->next == NULL && idx != 0) {
                prev->next = NULL;
            }

            // middle item
            if (entry->next != NULL && idx != 0) {
                prev->next = entry->next;
            }

            // free the deleted entry
            free(entry->key);
            free(entry->value);
            free(entry);

            return;
        }

        // walk to next
        prev = entry;
        entry = (Entry *) prev->next;

        idx++;
    }
}

static void free_entry(Entry *entry){
    if (entry == NULL){
        return;
    }
    free(entry->key);
    free(entry->value);
    Entry *next = (Entry *) entry->next;
    free(entry);
    free_entry(next);
}


void ht_free(HashTable *hashtable){
    if (hashtable == NULL){
        return;
    }
    for (int i = 0; i < TABLE_SIZE; i++){
        free_entry(hashtable->entries[i]);
    }
    free(hashtable->entries);
    free(hashtable);
}


