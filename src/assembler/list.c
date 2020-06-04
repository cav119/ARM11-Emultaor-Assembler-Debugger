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
}
