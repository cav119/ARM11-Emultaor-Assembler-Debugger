#ifndef DEQUE_H
#define DEQUE_H
#include <stdlib.h>
#include <stdbool.h>


// Free function for an element of the list
typedef void (*free_f) (void *);

// A linked list node
typedef struct list_node {
    void *elem;
    struct list_node *next;
    struct list_node *prev;
} ListNode;

// The definition of a list
typedef struct list {
    ListNode *head;
    ListNode *tail;
    int size;
} List;


// returns an empty heap-allocated list, with NULL values
List *create_list(void);

// Gets the head of the list
void *list_get_head(List *list);

// Gets the last element of the lists
void *list_get_last(List *list);

// Deletes the head of a list
void list_del_head(List *list, void (*free_fun)(void *));

// Deletes the last of a list
void list_del_last(List *list, void (*free_fun)(void *));

// Gets the element at index if it's within bounds, else NULL
void *list_get_index(List *list, int index);

// adds an element to the back of the list
void list_append(List *list, void *elem);

// Gets an element based on comp function, not sure if it actually serves
// a purpose since you already have an identical key
void *list_get(List *list, void *key, int (*comp)(const void *, const  void *));

// Deletes an element based on a comparator function
// and returns whether it did indeed find it
bool delete_by_key(List *List, void *key, int (*comp)(const void *, const  void *), 
                    void (*free_fun)(void *));

// Applies function to all list elements
void list_for_each(List *list, void (*fun)(void *)); 

// Frees and destroys the list using the given free function, then destroys the List itself
void list_destroy(List *list, void (*free_fun)(void *)); 

#endif
