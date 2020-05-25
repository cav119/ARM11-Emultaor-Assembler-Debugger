#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "utilities.h"

#define PC 15

CpuState *cpu_state_init(void) {
	CpuState *cpu_state = (CpuState *) malloc(sizeof(CpuState));
	check_ptr_not_null(cpu_state, "Could not allocate cpu state memory");

	// allocate and init registers to 0
	uint8_t *allocated_regs = calloc(NUM_REGISTERS, sizeof(uint32_t));
	check_ptr_not_null(allocated_regs, "Could not allocate and initialise registers");
	cpu_state->registers = (uint32_t *) allocated_regs;

	// allocate and init memory to 0
	uint8_t *allocated_memory = calloc(MEMORY_SIZE, sizeof(uint8_t));
	check_ptr_not_null(allocated_memory, "Could not allocate and initialise RAM memory");
	cpu_state->memory = (uint8_t *) allocated_memory;
	// init of pipeline
	return cpu_state;
}

void cpu_state_free(CpuState *cpu_state){
	free(cpu_state->memory);
	free(cpu_state->registers);
	free(cpu_state);
}

void set_flag(CpuState *cpuState, flag flag, bool set){
    if (flag > 3){
        print_error_exit("Invalid flag.");
    }
    int mask = (1 << (MAX_INDEX - flag));
    cpuState->registers[CPSR] = set ?
            cpuState->registers[CPSR] | mask : cpuState -> registers[CPSR] & ~(mask);
}

bool get_flag(CpuState *cpuState, flag flag){
    int mask = (1 << (MAX_INDEX - flag));
    return cpuState->registers[CPSR] & mask;
}


// DEBUGGING, should probably move to utilities.c later

void print_registers(CpuState *cpu_state){
    for (int i = 0; i < NUM_REGISTERS - 4; i++){
        printf("reg[%d] = 0x%.8x\n", i, cpu_state->registers[i]);
    }
	printf("PC = 0x%.8x\nCPSR = 0x%.8x\n", 
		cpu_state->registers[PC], 
		cpu_state->registers[CPSR]
	);
}


void print_memory(CpuState *cpu_state, int from, int to){
    for (int i = from; i < to; i++){
        printf("M[0x%.8x] = 0x%.8x\n", i, cpu_state->memory[i]);
    }
}

uint32_t *get_pc(CpuState *cpu_state){
	return (cpu_state->registers) + PC;
}

uint32_t *offset_pc(int32_t offset, CpuState *cpu_state){
	uint32_t *pc = get_pc(cpu_state); 
	*pc = ((int32_t) *pc + offset);
	return pc;
}

uint32_t *increment_pc(CpuState *cpu_state)
{
	uint32_t *pc = get_pc(cpu_state); 
	*pc = *pc + 4;
	return pc;
}

