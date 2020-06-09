#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H

// Implementation of ArrayList from Java

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define INIT_SIZE (100)


typedef struct {
    void *elements;
    int size;
    int max_size;     
} ArrayList;

ArrayList *arrlist_init(void);

// checks if an index is valid
static bool valid_index(ArrayList *list, int index);

// Pushes an element at the end of the list
void arrlist_push(ArrayList *list, void *elem);

// Gets an element at index if valid, NULL otherwise
void *arrlist_get(ArrayList *list, int index);
#endif
