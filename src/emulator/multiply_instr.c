#include <stdint.h>
#include "utilities.h"

#define ACCUMULATE_BITS bit_mask(instr->code, 21)
#define REG_D_BITS process_mask(instr->code, 16, 19)
#define REG_N_BITS process_mask(instr->code, 12, 15)
#define REG_S_BITS process_mask(instr->code, 8, 11)
#define REG_M_BITS process_mask(instr->code, 0, 3)

void execute_multiply_instruction(Instruction* instr, CpuState *cpu_state){
    uint32_t set = bit_mask(instr->code, 20);

    uint32_t result = cpu_state->registers[REG_M_BITS] * cpu_state->registers[REG_S_BITS];
    if (ACCUMULATE_BITS){
        result += cpu_state->registers[REG_N_BITS];
    }

    if (set) {
        set_CPSR_flag(cpu_state, N, bit_mask(result, 31));
        if (!result){
            set_CPSR_flag(cpu_state, Z, 1);
        }
    }

    cpu_state->registers[REG_D_BITS] = result;
}
