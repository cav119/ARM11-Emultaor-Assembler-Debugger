#include "array_list.h"


// Checks if the arrayList reference is NULL
#define ARR_REF_CHECK(ref) \
 do { if (ref == NULL) { perror("You gave me a NULL ArrayList"); return NULL; } }\
    while (0)

static bool valid_index(ArrayList *list, int index){
    ARR_REF_CHECK(list);
    return index >= 0 && index < list->size;
}


ArrayList *arrlist_init(void){
    ArrayList *list = calloc(1, sizeof(ArrayList));
    if (!list) {
        perror("Error allocating memory for array list");
        exit(EXIT_FAILURE);
    }
    list->max_size = INIT_SIZE;
    list->elements = calloc(INIT_SIZE, sizeof(element));
    if (!list->elements) {
        perror("Error allocating memory for elements in array list");
        exit(EXIT_FAILURE);
    }
    return list; 
}


void arrlist_append(ArrayList *list, void *elem) {
    if (list == NULL){
        return;
    }
    if (list->size == list->max_size){
        list->max_size *= SIZE_MULTIPLIER;
        list->elements = realloc(list->elements, list->max_size * sizeof(element));
        if (!list->elements) {
            perror("Error reallocating memory for list's elements");
            exit(EXIT_FAILURE);
        }
        list->elements[list->size] = elem;
        list->size += 1;
        return;
    }
    list->elements[list->size] = elem;
    list->size += 1;
}


void *arrlist_get(ArrayList *list, int index) {
    ARR_REF_CHECK(list);
    if (!valid_index(list, index)){
        return NULL;
    }
    return list->elements[index];
}
    
void arrlist_destroy(ArrayList *list) {
    free(list->elements);
    free(list);
}


void arrlist_destroy_free(ArrayList *list, void (*free_fun)(void *)) {
    for (int i = 0; i < list->size; i++){
        free_fun(list->elements[i]);
    }
    free(list->elements);
    free(list);
}
