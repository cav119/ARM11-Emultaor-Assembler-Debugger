#include <string.h>
#include <stdlib.h>
#include "asm_utilities.h"
#include <stdio.h>


uint32_t process_mask(uint32_t n, uint8_t start_pos, uint8_t end_pos) {
    uint32_t mask = (1 << (end_pos + 1 - start_pos)) - 1;
    return (n >> start_pos) & mask;
}

uint32_t set_bit(uint32_t n, int pos) {
    n |= (1 << pos);
    return n;
}

int read_reg_num(char *reg) {
    if (reg == 0){
        return 0;
    }
    return  atoi(strtok(reg, "r"));
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

size_t hash_str_size(char *str){
    if (str == NULL){
        return 0;
    }
    return sizeof(char) * (strlen(str));
}


char *str_clone(const char *src){
    if (src == NULL){
        perror("You tried to clone NULL string");
    }
    int len = strlen(src);
    char *clone = malloc(sizeof(char) * (len + 1));
    if (!clone) {
        perror("Error allocating memory for string clone");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < len; i++){
        clone[i] = src[i];
    }
    clone[len] = '\0';
    return clone;
}

