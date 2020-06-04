#include <stdlib.h>

/* DISCLAIMER!
// If you add an element to the list, 
// on list_destroy it WILL be freed!
// so do not count on it existing afterwards!
// Use duplicates
*/

// Free function for an element of the list
typedef void (*free_f)(void *)

// A linked list node
typedef struct list_node {
    void *elem;
    ListNode *next;
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
void *list_del_head(List *list, void (*free_fun)(void *));

// Deletes the last of a list
void *list_del_last(List *list, void (*free_fun)(void *));

// Gets the element at index if it's within bounds, else NULL
void *list_get(List *list, int index);

// adds an element to the list
void list_add(List *list, void *elem); 

// Deletes an element based on index, if it is in range
void delete_elem(List *list, int index, void (*free_fun)(void *));

// Deletes an element based on a comparator function
void delete_by_key(List *list, void *key, int (*comp)(const void *, const  void *));

// Applies function to all list elements
void list_for_each(List *list, void (*fun)(void *)); 

// Frees and destroys the list using the given free function, then destroys the List itself
void list_destroy(List *list, void (*free_fun)(void *)); 

