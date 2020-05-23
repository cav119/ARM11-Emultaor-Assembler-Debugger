#include <stdio.h>
#include <stdlib.h>

#include "utilities.h"
#include "pipeline.h"

void print_error_exit(char error_msg[]) {
	printf("%s\n", error_msg);
	exit(EXIT_FAILURE);
}

void print_bits_32(uint32_t n) {
    uint32_t mask = 1 << 31;
    for (int i = 0; i < 32; i++){
        if (n & mask) {
            printf("1");
        }
        else {
            printf("0");
        }
        n <<= 1;
    }
    printf("\n");
}

uint32_t process_mask(uint32_t n, uint8_t start_pos, uint8_t end_pos){
    if (start_pos > end_pos){
        print_error_exit("Start bit of mask cannot be bigger than end bit of mask");
    }
    // makes a mask with 1s until endpos
    uint32_t mask = (1 << end_pos + 1) - 1;
    // substracts a number with 1s until startpos to effectively make the proper mask
    mask -= (1 << start_pos) - 1;
    uint32_t res = n;
  /*  printf("res:  ");
    print_bits_32(res);
    printf("mask: ");
    print_bits_32(mask); */
    // processes result
    res &= mask;
    // shifts result to the beginning for convenience
    res >>= start_pos;
  //  printf("res:  ");
   // print_bits_32(res);
    return res;
}

void check_ptr_not_null(void *ptr, char error_msg[]) {
	if (!ptr) {
		print_error_exit(error_msg);
	}
}
char *instr_to_string(instruction_type type){
	switch (type){
		case data_process:
			return "DATA_PROCESS";
		case multiply: 
			return "MULTIPLY";
		case single_data_transfer:
			return "SINGLE_DATA_TRANSFER";
		case branch:
			return "BRANCH";	
		default:
			return "WRONG_INPUT";
	}
}

