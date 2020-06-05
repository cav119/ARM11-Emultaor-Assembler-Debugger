#include "list.h"



List *create_list(void){
    return calloc(1, sizeof(List));
}

void *list_get_head(List *list){
    return list->head->elem;
}

void *list_get_last(List *list){
    return list->tail->elem;
}

void list_del_head(List *list, void (*free_fun)(void *)){
    if (!list->head){
        return;
    }
    ListNode *snd = list->head->next;
    free_fun(list->head->elem);
    free(list->head);
    list->head = snd;
    if (list->head){
        list->head->prev = NULL;
    }
}

void list_del_last(List *list, void (*free_fun)(void *)){
    if (!list->tail){
        return;
    }
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

void *list_get(List *list, int index){
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

void list_add(List *list, void *elem){
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
        list->size += 1;
        return;
   }
   // List with more than 2 values
    node->prev = list->tail;
    list->tail->next = node;
    list->tail = node;
    list->size += 1;
}

void delete_elem(List *list, int index, free_f freer){
    if (!index_ok(list, index)){
      return ;
    }
    ListNode *curr = list->head;
    for (int i = 0; i < index; i++, curr = curr->next ){}
    // curr is now at correct index
    if (index == 0){
        list->head = list->head->next;
        if (list->head == list->tail){
            // only 1 element left
            list->tail = NULL;
        }
    }
    if (index == list->size - 1){
        if (list->head == list->tail->prev){
            list->tail = NULL;
        }
        else {
            list->tail = list->tail->prev;
        }
    }
    freer(curr->elem);
    free(curr);
    list->size -= 1;
}

bool delete_by_key(List *list, void *key, int (*comp)(const void *, const  void *), 
                    void (*free_fun)(void *)){
    ListNode *curr;
    for (curr = list->head; curr; curr = curr->next){
        if (comp(curr->elem, key) == 0){
            ListNode *prev = curr->prev;
            ListNode *next = curr->next;
            free_fun(curr->elem);
            free(curr);
            if (!prev){
                // was the head
                list->head = next; 
            }
            else if (!next){
                // was the tail
                if (list->head == prev){
                    // only 2 elements inside
                    list->tail = NULL;
                }
                else {
                    list->tail = prev;
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

}

void list_destroy(List *list, free_f freer){
   destroy_node(list->head, freer); 
}


