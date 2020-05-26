#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "utilities.h"
#include "pipeline_executor.h"

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
    printf("Registers:\n");
	for (int i = 0; i < NUM_REGISTERS; i++){
		uint32_t val = cpu_state->registers[i];
		if (i == 13 || i == 14){
		    continue;
		}
		if (i != CPSR && i != PC){
			printf("$%d:            %d (0x%.8x)\n", i, val, val);
		}
		else if (i == PC){
			printf("PC:            %d (0x%.8x)\n", val, val);
		}
		else {
			printf("CPSR:          %d (0x%.8x)\n", val, val);
		}
	}
}




void print_memory(CpuState *cpu_state, int from, int to){
    for (int i = from; i < to; i++){
        printf("M[0x%.8x] = 0x%.8x\n", i, cpu_state->memory[i]);
    }
}

void print_nonzero_little_endian_memory(CpuState *cpu_state, size_t bytes){
    // it is assumed memory is an array of bytes
    // but addresses are all 32-bit
    printf("Non-zero memory: \n");
    for (int i = 0; i < bytes; i += 4){
        uint32_t val = fetch(i, cpu_state);
        if (val != 0X0){
            printf("0x%.8x: 0x%.8x\n", i, val);
        }
    }
}

void print_nonzero_big_endian_memory(CpuState *cpu_state, size_t bytes){
    printf("Non-zero memory: \n");
    for (int i = 0; i < bytes; i += 4){
        uint8_t *memory = cpu_state->memory;
        uint8_t byte1 = memory[i];
        uint8_t byte2 = memory[i+1];
        uint8_t byte3 = memory[i+2];
        uint8_t byte4 = memory[i+3];
        uint32_t val = (byte1 << 24) + (byte2 << 16) + (byte3 << 8) + byte4;
        if (val != 0X0){
            printf("0x%.8x: 0x%.8x\n", i, val);
        }
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

