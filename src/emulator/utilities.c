#include <stdio.h>
#include <stdlib.h>
#include "utilities.h"

void print_error_exit(char error_msg[]) {
	printf("%s\n", error_msg);
	exit(EXIT_FAILURE);
}


void check_valid_memory_access(CpuState *cpu_state, uint32_t address) {
	if (address > MEMORY_SIZE - 4) {
        printf("Error: Out of bounds memory access at address 0x%.8x\n", address);
        print_registers(cpu_state);
        print_nonzero_big_endian_memory(cpu_state, MEMORY_SIZE);
        exit(EXIT_FAILURE);
    }
}


void print_bits_32(uint32_t n) {
    uint32_t mask = 1 << 31;
    for (int i = 0; i < 32; i++) {
        if (n & mask) {
            printf("1");
        }
        else {
            printf("0");
        }
        n <<= 1;
        if (i + 1 % 8 == 0) {
            printf(" ");
        }
    }
    printf("\n");
}


uint32_t process_mask(uint32_t n, uint8_t start_pos, uint8_t end_pos) {
    if (start_pos > end_pos){
        print_error_exit("Start bit of mask cannot be bigger than end bit of mask");
    }
    uint32_t mask = (1 << (end_pos + 1)) - 1;
    mask -= (1 << start_pos) - 1;
    uint32_t res = n;
    res &= mask;
    res >>= start_pos;

    return res;
}


uint32_t bit_mask(uint32_t n, uint8_t pos) {
    return process_mask(n, pos, pos);
}


void check_ptr_not_null(void *ptr, char error_msg[]) {
	if (!ptr) {
		print_error_exit(error_msg);
	}
}


char *instr_to_string(instruction_type type) {
	switch (type) {
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


void set_CPSR_flag(CpuState* cpu_state, flag flag, bool set) {
    uint32_t mask = (1 << (31 - flag));
    cpu_state->registers[CPSR] = set ? 
        cpu_state->registers[CPSR] | mask : cpu_state->registers[CPSR] & ~mask;
}


bool check_CPSR_cond(uint8_t cond, CpuState* cpu_state) {
    switch (cond) {
        case 0:
            return get_flag(cpu_state, Z);
        case 1:
            return !get_flag(cpu_state, Z);
        case 10:
            return get_flag(cpu_state, N) == get_flag(cpu_state, V);
        case 11:
            return get_flag(cpu_state, N ) != get_flag(cpu_state, V);
        case 12:
            return !get_flag(cpu_state, Z) &&
                    (get_flag(cpu_state, N) == get_flag(cpu_state, V));
        case 13:
            return get_flag(cpu_state, Z) ||
            (get_flag(cpu_state, N ) != get_flag(cpu_state, V));
        case 14:
            return true;
        default:
            print_error_exit("Undefined CPSR condition");
    }
    
    return false;
}
