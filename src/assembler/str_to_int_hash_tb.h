#include <stdint.h>

typedef struct dict* Dict;



/* create a new empty dictionary */
Dict dict_create(void);

/* destroy a dictionary */
void dict_destroy(Dict);

/* insert a new key-value pair into an existing dictionary */
void dict_insert(Dict, const char *key, uint32_t value);

/* return the most recently inserted value associated with a key */
/* or 0 if no matching key is present */
uint32_t dict_search(Dict, const char *key);

/* delete the most recently inserted record with the given key */
/* if there is no such record, has no effect */
void dict_delete(Dict, const char *key);