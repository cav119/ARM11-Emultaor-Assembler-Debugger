#include <stdint.h>
#include "utilities.h"

#define ACCUMULATE_BITS(bits) bit_mask(bits, 21)
#define REG_D_BITS(bits) process_mask(bits, 16, 19)
#define REG_N_BITS(bits) process_mask(bits, 12, 15)
#define REG_S_BITS(bits) process_mask(bits, 8, 11)
#define REG_M_BITS(bits) process_mask(bits, 0, 3)


void execute_multiply_instruction(Instruction* instr, CpuState *cpu_state){
    uint32_t set = bit_mask(instr->code, 20);

    uint32_t result = cpu_state->registers[REG_M_BITS(instr->code)] * 
        cpu_state->registers[REG_S_BITS(instr->code)];
        
    if (ACCUMULATE_BITS(instr->code)) {
        result += cpu_state->registers[REG_N_BITS(instr->code)];
    }

    if (set) {
        set_CPSR_flag(cpu_state, N, bit_mask(result, 31));
        if (!result) {
            set_CPSR_flag(cpu_state, Z, 1);
        }
    }

    cpu_state->registers[REG_D_BITS(instr->code)] = result;
}
