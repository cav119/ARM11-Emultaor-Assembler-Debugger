#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdint.h>

#define MEMORY_SIZE 65536
#define NUM_REGISTERS 17
#define PC 15
#define CPSR 16
#define MAX_INDEX 31


// Prints a given error message and exits the program
void print_error_exit(char error_msg[]);
void print_bits_32(uint32_t n);
uint32_t process_mask(uint32_t n, uint8_t start_pos, uint8_t end_pos);

// Checks if a pointer is defined. Otherwise, prints message and exits
void check_ptr_not_null(void *ptr, char error_msg[]);


#endif