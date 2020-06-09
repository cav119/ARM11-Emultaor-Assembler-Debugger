#include "array_list.h"

typedef element void *;

#define CHECK_ARR_REF(ref) do {\
    if (ref == NULL) { perror("You gave me a null array reference"); return NULL; } }\
        while(0)


static bool valid_index(ArrayList *list, int index){
   CHECK_ARR_REF(list); 
   return index >= 0 && index < list->size;
}

ArrayList *arrlist_init(void){
    ArrayList *arr = calloc(1, sizeof(ArrayList));
    arr->max_size = INIT_SIZE; 
    arr->elements = calloc(INIT_SIZE, sizeof(element));
    return arr;
}

void arrlist_push(ArrayList *list, void *elem) {
    CHECK_ARR_REF(list);
    if (size == max_size){
        list->max_size *= 2;
        list = realloc(list, sizeof(element) * list->max_size);
        list->size += 1;
        list->elements[size] = elem;
    }
    else {
        list->elements[size] = elem;
        list->size += 1;
    }
}

void *arrlist_get(ArrayList *list, int index) {
    CHECK_ARR_REF(list);
    if (!valid_index(list, index)){
        return NULL;
    }
    return list->elements[index];
}


