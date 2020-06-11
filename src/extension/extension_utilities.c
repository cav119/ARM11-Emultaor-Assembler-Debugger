#include "extension_utilities.h"

void print_bits_32(uint32_t bits) {
    uint32_t mask = 1 << 31;
    for (int i = 0; i < 32; i++){
        if (mask & bits) {
            printf("1");
        }
        else {
            printf("0");
        }
        bits <<= 1;
    }
    printf("\n");
}

