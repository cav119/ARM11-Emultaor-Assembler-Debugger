#ifndef UTILITIES_H
#define UTILITIES_H
#include <stdbool.h>
#include <stdint.h>
#include "cpu_state.h"
#include "pipeline_data.h"

#define MEMORY_SIZE 65536
#define NUM_REGISTERS 17
#define PC 15
#define CPSR 16
#define MAX_INDEX 31

// Prints a given error message and exits the program
void print_error_exit(char error_msg[]);

// Checks that an address is in bounds (should be called before any memory access) 
bool check_valid_memory_access(CpuState *cpu_state, uint32_t address);

// Returns the masked bit pattern from start to end of a number, 
// shifted to the beggining for readability
uint32_t process_mask(uint32_t n, uint8_t start_pos, uint8_t end_pos);

// Checks if a bit is on
uint32_t bit_mask(uint32_t n, uint8_t pos);

// Checks if a pointer is defined. Otherwise, prints message and exits
void check_ptr_not_null(void *ptr, char error_msg[]);

// Sets the given CPSR flag as the given bool value
void set_CPSR_flag(CpuState *cpu_state, flag flag, bool set);

// Checks the status of the CPSR given a boolean condition
bool check_CPSR_cond(uint8_t cond, CpuState *cpu_state);

#endif
