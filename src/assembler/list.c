#include "list.h"



List *create_list(void){
    return calloc(1, sizeof(List));
}

void *list_get_head(List *list){
    return list->head->elem;
}

void *list_get_last(List *list){
    if (!list->head){
        return list->head->elem;
    }
    return list->tail->elem;
}

void list_del_head(List *list, void (*free_fun)(void *)){
    if (!list->head){
        return;
    }
    list->size -= 1;
    ListNode *snd = list->head->next;
    free_fun(list->head->elem);
    free(list->head);
    list->head = snd;
    if (snd == list->tail){
        list->tail = NULL;
    }
    if (list->head){
        list->head->prev = NULL;
    }
}

void *list_get(List *list, void *key, int (*comp)(const void *, const  void *)){
    ListNode *curr;
    for (curr = list->head; curr; curr = curr->next){
        if (comp(curr->elem, key) == 0){
            return curr->elem;
        }
    }
    return NULL;
}

void list_del_last(List *list, void (*free_fun)(void *)){
    if (!list->tail){
        if (list->head){
            list->size -= 1;
            ListNode *head = list->head;
            free_fun(head->elem);
            free(head);
            list->head = NULL;
        }
        else {
            // empty list
            return;
        }
    }
    list->size -= 1;
    ListNode *last = list->tail;
    ListNode *new_last= last->prev;
    free_fun(last->elem);
    free(last);
    // if there are only two nodes in the list, make tail NULL
    if (new_last == list->head){
        list->tail = NULL;
        return;
    }
    list->tail = new_last;
    new_last->next = NULL;
}

bool index_ok(List *list, int index){
    return index >= 0 && index < list->size;
}

static void *list_get_index(List *list, int index){
    if (!index_ok(list, index)){
      return NULL;  
    }
    ListNode *curr = list->head;
    for (int i = 0; i < index; i++, curr = curr->next ){}
    // curr is now at correct index
    return curr->elem;
}

// Returns a new node with NULL fields
static ListNode* node_alloc(void){
    return calloc(1, sizeof(ListNode));
}

void list_append(List *list, void *elem){
   ListNode* node = node_alloc();
   node->elem = elem;
   // Empty list
   if (list->head == NULL){
    list->head = node;
    list->size += 1;
    return;
   }
   // List only with head
   if (list->tail == NULL){
        list->head->next = node;
        list->tail = node;
        node->prev = list->head;
        list->size += 1;
        return;
   }
   // List with more than 2 values
    node->prev = list->tail;
    list->tail->next = node;
    list->tail = node;
    list->size += 1;
}



bool delete_by_key(List *list, void *key, int (*comp)(const void *, const  void *), 
                    void (*free_fun)(void *)){
    ListNode *curr;
    for (curr = list->head; curr; curr = curr->next){
        if (comp(curr->elem, key) == 0){
            list->size -= 1;
            ListNode *prev = curr->prev;
            ListNode *next = curr->next;
            free_fun(curr->elem);
            free(curr);
            if (!prev){
                // was the head
                list->head = next;
                list->head->prev = NULL;
                if (next == list->tail){
                    list->tail = NULL;
                    list->head->next = NULL;
                }
            }
            else if (!next){
                // was the tail
                if (list->head == prev){
                    // only 2 elements inside
                    list->tail = NULL;
                    list->head->next = NULL;
                }
                else {
                    list->tail = prev;
                    list->tail->next = NULL;
                }
            }
            else {
                prev->next = next;
                next->prev = prev;
            }
            return true;
        }
    }
    return false;
}

void list_for_each(List *list, void (*fun)(void *)){
    for (ListNode *curr = list->head; curr; curr = curr->next){
        fun(curr->elem);
    }
}

static void destroy_node(ListNode *node, free_f freer){
    if (!node){
        return;
    }
    freer(node->elem);
    destroy_node(node->next, freer);
    free(node);
}

void list_destroy(List *list, free_f freer){
   destroy_node(list->head, freer);
   free(list);
}


