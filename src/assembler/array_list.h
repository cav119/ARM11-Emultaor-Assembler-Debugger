#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H


#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

// Initial size of an arrayList
#define INIT_SIZE (100)

// The multiplier for the time when list gets filled
#define SIZE_MULTIPLIER (2)

typedef void *element;

typedef struct array_list {
    void **elements;
    int size;
    int max_size;
} ArrayList;

// Initialize an empty array list
ArrayList *arrlist_init(void);

// Appends an element at the end of the list
void arrlist_append(ArrayList *list, void *elem);

//  Gets the element at index or NULL if invalid
void *arrlist_get(ArrayList *list, int index);

// Destroys the arraylist AND doesn't touch array content
void arrlist_destroy(ArrayList *list);

// Destroys the arraylist and frees the elements inside the array with given
// free function
void arrlist_destroy_free(ArrayList *list, void (*free_fun)(void *));
#endif
