#include <string.h>
#include <stdlib.h>
#include "asm_utilities.h"
#include <stdio.h>

// char **get_instr_tokens(char instr[]) {
//     char **words = calloc(5, 512);
//     char *arr = strtok(instr, " ,:\n");
//     int i = 0;
//     while (arr != NULL) {
//         words[i] = arr;
//         arr = strtok(NULL, " ,:\n");
//         i ++;
//     }
//     return words;
// }

// }


uint32_t process_mask(uint32_t n, uint8_t start_pos, uint8_t end_pos) {
    uint32_t mask = (1 << (end_pos + 1 - start_pos)) - 1;
    return (n >> start_pos) & mask;
}

int32_t setBit(uint32_t n, int pos){
    n |= (1 << pos);
    return n;
}

void print_bits(uint32_t x) {
    int i;
    uint32_t mask = 1 << 31;

    for (i=0; i<32; ++i) {
        if ((x & mask) == 0) {
            printf("0");
        }
        else {
            printf("1");
        }
        x = x << 1;
    }
    printf("\n");
}

void check_pointer(void *ptr, char *error_msg){
    if (ptr == NULL){
       puts(error_msg); 
       exit(EXIT_FAILURE);
    }
}

size_t str_size(char *str){
    if (str == NULL){
        return 0;
    }
    return sizeof(char) * (strlen(str) + 1);
}
