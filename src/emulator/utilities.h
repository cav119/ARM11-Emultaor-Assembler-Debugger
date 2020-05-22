#ifndef UTILITIES_H
#define UTILITIES_H

#define MEMORY_SIZE 65536
#define NUM_REGISTERS 17
#define PC 15
#define CPSR 16
#define MAX_INDEX 31


// Prints a given error message and exits the program
void print_error_exit(char error_msg[]);

// Checks if a pointer is defined. Otherwise, prints message and exits
void check_ptr_not_null(void *ptr, char error_msg[]);


#endif