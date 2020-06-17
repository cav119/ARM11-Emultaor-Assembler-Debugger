#include "extension_utilities.h"

char *print_bits_32(uint32_t bits) {
    char *bit_str = malloc(sizeof(char) * 33);
    uint32_t mask = 1 << 31;
    for (int i = 0; i < 32; i++){
        if (mask & bits) {
            bit_str[32 - i] = '1';
            // printf("1");
        }
        else {
            bit_str[32 - i] = '0';
            // printf("1");
        }
        bits <<= 1;
    }
    // printf("\n");
    bit_str[32] = '\0';

    return bit_str;
}

