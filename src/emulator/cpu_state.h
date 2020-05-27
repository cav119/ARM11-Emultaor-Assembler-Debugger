#ifndef CPU_STATE_H
#define CPU_STATE_H
#include <stdlib.h>

// CpuState struct
typedef struct {
	uint32_t *registers;
	uint8_t *memory;
} CpuState;

// Flag enum
typedef enum {
	N, 
	Z, 
	C, 
	V
} flag;


// Create a new CPU and an initialise the registers/memory to zero
CpuState *cpu_state_init(void);

// Frees the allocated memory in the CpuState
void cpu_state_free(CpuState *cpu_state);

// Prints fields of memory that are non-zero in little endian
// cpu has memory of size "bytes"
void print_nonzero_little_endian_memory(CpuState *cpu_state, size_t bytes);

// Prints fields of memory that are non-zero in big endian
// cpu has memory of size "bytes"
void print_nonzero_big_endian_memory(CpuState *cpu_state, size_t bytes);

// Should probably move these two funcs to utilities.c
// Prints the current state of all registers for debugging
void print_registers(CpuState *cpu_state);

// Prints the current state of memory for debugging
void print_memory(CpuState *cpu_state, int from, int to);

// Sets CPSR flags
void set_flag(CpuState *cpu_state, flag flag, bool set);

// Gets CPSR flag status
bool get_flag(CpuState *cpu_state, flag flag);

// Make PC go go to instruction with offset
uint32_t *offset_pc(int32_t offset, CpuState *cpu_state);

// Increments PC to next instruction (adding 4 bytes)
uint32_t *increment_pc(CpuState *cpu_state);

#endif
