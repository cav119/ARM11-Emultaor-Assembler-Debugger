

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "hash_table.h"


#define INITIAL_SIZE (1024)
#define GROWTH_FACTOR (2)
#define MAX_LOAD_FACTOR (1)


/* dictionary initialization code used in both dict_create and grow */
Dict dict_create_internal(int size, int (*comp)(const void *, const void *), size_t value_size)
{
    Dict d;
    int i;

    d = malloc(sizeof(*d));

    assert(d != 0);

    d->size = size;
    d->n = 0;
    d->table = malloc(sizeof(struct elt *) * d->size);
    d->comp = comp;
    d->value_size = value_size;
    assert(d->table != 0);

    for(i = 0; i < d->size; i++) d->table[i] = 0;

    return d;
}

// creates a dictionary with a comparator function
Dict dict_create(int (*comp)(const void *, const void *), size_t value_size) {
    return dict_create_internal(INITIAL_SIZE, comp, value_size);
}

void dict_destroy(Dict d)
{
    int i;
    struct elt *e;
    struct elt *next;

    for(i = 0; i < d->size; i++) {
        for(e = d->table[i]; e != 0; e = next) {
            next = e->next;
            free(e->value);
            free(e->key);
            free(e);
        }
    }

    free(d->table);
    free(d);
}

#define MULTIPLIER (97)

static unsigned long hash_function(const char *s, size_t value_size){
    unsigned const char *us;
    unsigned long h;

    h = 0;
    size_t curr_size = 0;
    for(us = (unsigned const char *) s; curr_size < value_size; us++) {
        h = h * MULTIPLIER + *us;
        curr_size++;
    }

    return h;
}

static void dict_grow(Dict d)
{
    Dict d2;            /* new dictionary we'll create */
    struct dict swap;   /* temporary structure for brain transplant */
    int i;
    struct elt *e;
    

    d2 = dict_create_internal(d->size * GROWTH_FACTOR, d->comp, d->value_size);

    for(i = 0; i < d->size; i++) {
        for(e = d->table[i]; e != 0; e = e->next) {
            /* note: this recopies everything */
            /* a more efficient implementation would
             * patch out the strdups inside dict_insert
             * to avoid this problem */
            dict_insert(d2, e->key, e->value);
        }
    }

    /* the hideous part */
    /* We'll swap the guts of d and d2 */
    /* then call dict_destroy on d2 */
    swap = *d;
    *d = *d2;
    *d2 = swap;

    dict_destroy(d2);
}

/* insert a new key-value pair into an existing dictionary */
void dict_insert(Dict d, hashkey key, hashvalue value)
{
    struct elt *e;
    unsigned long h;

    assert(key);

    e = malloc(sizeof(*e));

    assert(e);

    e->key = strdup(key);
    e->value = value; 

    h = hash_function((char *)key, d->value_size) % d->size;


    e->next = d->table[h];
    d->table[h] = e;

    d->n++;

    /* grow table if there is not enough room */
    if(d->n >= d->size * MAX_LOAD_FACTOR) {
        dict_grow(d);
    }
}

/* return the most recently inserted value associated with a key */
/* or 0 if no matching key is present */
void *dict_search(Dict d, hashkey key)
{
    struct elt *e;
    for(e = d->table[hash_function((char *) key, d->value_size) % d->size]; e != 0; e = e->next) {
        if(d->comp(e->key, key)) {
            /* got it */
            return e->value;
        }
    }

    return 0;
}

/* delete the most recently inserted record with the given key */
/* if there is no such record, has no effect */
void dict_delete(Dict d, hashkey key)
{
    struct elt **prev;          /* what to change when elt is deleted */
    struct elt *e;              /* what to delete */

    for(prev = &(d->table[hash_function((char *) key, d->value_size) % d->size]); 
        *prev != 0; 
        prev = &((*prev)->next)) {
        if (d->comp((*prev)->key, key)){ 
            /* got it */
            e = *prev;
            *prev = e->next;

            free(e->key);
            free(e);

            return;
        }
    }
}
