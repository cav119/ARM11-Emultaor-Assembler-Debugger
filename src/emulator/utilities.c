#include <stdio.h>
#include <stdlib.h>
#include "utilities.h"


void print_error_exit(char error_msg[]) {
    printf("%s\n", error_msg);
    exit(EXIT_FAILURE);
}


bool check_valid_memory_access(CpuState *cpu_state, uint32_t address) {
    if (address > MEMORY_SIZE - 4 || address < 0) {
        printf("Error: Out of bounds memory access at address 0x%.8x\n", address);
        return false;
    }
    return true;
}


uint32_t index_little_endian_bytes(uint8_t *ptr) {
    return *ptr | (*(ptr + 1) << 8) | (*(ptr + 2) << 16) | (*(ptr + 3) << 24);;
}

uint32_t index_big_endian_bytes(uint8_t *ptr){
    return *ptr << 24 | ptr[1] << 16 | ptr[2] << 8 | ptr[3];
}


uint32_t process_mask(uint32_t n, uint8_t start_pos, uint8_t end_pos) {
    uint32_t mask = (1 << (end_pos + 1 - start_pos)) - 1;
    return (n >> start_pos) & mask;
}


uint32_t bit_mask(uint32_t n, uint8_t pos) {
    return (n >> pos) & 1;
}


void check_ptr_not_null(void *ptr, char error_msg[]) {
    if (!ptr) {
        print_error_exit(error_msg);
    }
}


void set_CPSR_flag(CpuState* cpu_state, flag flag, bool set) {
    uint32_t mask = (1 << (31 - flag));
    cpu_state->registers[CPSR] = set ? 
        cpu_state->registers[CPSR] | mask : cpu_state->registers[CPSR] & ~mask;
}


bool check_CPSR_cond(uint8_t cond, CpuState* cpu_state) {
    switch (cond) {
        case EQ: /* EQUAL */
            return get_flag(cpu_state, Z);
        case NE: /* NOT EQUAL */
            return !get_flag(cpu_state, Z);
        case GE: /* GREATER THAN OR EQUAL */
            return get_flag(cpu_state, N) == get_flag(cpu_state, V);
        case LT: /* LESS THAN */
            return get_flag(cpu_state, N) != get_flag(cpu_state, V);
        case GT: /* GREATER THAN */
            return !get_flag(cpu_state, Z) && (get_flag(cpu_state, N) == get_flag(cpu_state, V));
        case LE: /* LESS THAN OR EQUAL */
            return get_flag(cpu_state, Z) || (get_flag(cpu_state, N) != get_flag(cpu_state, V));
        case AL: /* ALWAYS */
            return true;
        default:
            print_error_exit("Undefined CPSR condition");
    }
    
    return false;
}
