#ifndef HASHTABLE_H
#define HASHTABLE_H
#define HASHTABLE_H
#include <stdint.h>

//base functionality imported from
//  http://www.cs.yale.edu/homes/aspnes/pinewiki/C(2f)HashTables.html


/* Creates a new Dictionary (~hashmap) with any type as key or value 
//    but always operatoes on pointers, so if you want (int, int)
//    you'll have to use (*int, *int)

*/

typedef struct dict *Dict;
typedef void *hashkey;
typedef void *hashvalue;

struct elt {
    struct elt *next;
    hashkey key;
    hashvalue value;
};

struct dict {
    int size;           /* size of the pointer table */
    int n;              /* number of elements stored */
    int (*comp)(const void *, const void *);
    struct elt **table;
};


/* create a new empty dictionary */
Dict dict_create(int (*comp)(const void *, const void *));

/* destroy a dictionary */
void dict_destroy(Dict);

/* insert a new key-value pair into an existing dictionary */
void dict_insert(Dict, hashkey key, hashvalue value);

/* return the most recently inserted value associated with a key */
/* or 0 if no matching key is present */
void * dict_search(Dict, hashkey key);

/* delete the most recently inserted record with the given key */
/* if there is no such record, has no effect */
void dict_delete(Dict, hashkey key);

#endif