#ifndef ASM_UTILITIES_H
#define ASM_UTILITIES_H
#include <stdint.h>
#include <stdlib.h>

// String utility functions (used throughout the program)

// Returns the substring of the given string between start and end inclusive
// char *substring(const char *str, int start, int end);

// Extracts a number from a string 
// char *get_num_from_str();


// Other utility functions

// Returns the masked bits (reused from emulator)
uint32_t process_mask(uint32_t n, uint8_t start_pos, uint8_t end_pos);

// Prints a 32 bit number as a binary string
void print_bits(uint32_t num);

// Checks pointer, exits if it is NULL
void check_pointer(void *ptr, char *error_msg);

// Returns the size of a string
size_t str_size(char *str);


#endif
