#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdint.h>
#include "pipeline.h"

#define MEMORY_SIZE 65536
#define NUM_REGISTERS 17
#define PC 15
#define CPSR 16
#define MAX_INDEX 31


// Prints a given error message and exits the program
void print_error_exit(char error_msg[]);
// prints a 32bit number
void print_bits_32(uint32_t n);
// returns the masked bit pattern from start to end of a number, shifted to the beggining for readability
uint32_t process_mask(uint32_t n, uint8_t start_pos, uint8_t end_pos);
// returns the string version of an instruction type
char* instr_to_string(instruction_type type);
// Checks if a pointer is defined. Otherwise, prints message and exits
void check_ptr_not_null(void *ptr, char error_msg[]);


#endif
