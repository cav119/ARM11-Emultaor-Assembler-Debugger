#ifndef CPU_STATE_H
#define CPU_STATE_H

// CpuState struct
typedef struct {
	uint32_t *registers;
	uint8_t *memory;
} CpuState;

// Flag enum
typedef enum {N, Z, C, V} flag;


// Create a new CPU and an initialise the registers/memory to zero
CpuState *cpu_state_init(void);

// Frees the allocated memory in the CpuState
void cpu_state_free(CpuState *cpu_state);


// should probably move these two funcs to utilities.c
// Prints the current state of all registers for debugging
void print_registers(CpuState *cpu_state);

// Prints the current state of memory for debugging
void print_memory(CpuState *cpu_state, int from, int to);


#endif