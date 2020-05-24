#include <stdint.h>
#include "utilities.h"


void execute_multiply_instruction(Instruction* instr, CpuState *cpu_state){
    uint32_t code = instr->code;
    uint32_t cond = process_mask(code, 28, 31);
    uint32_t accumulate = bit_mask(code, 21);
    uint32_t set = bit_mask(code, 20);
    uint32_t Rd = process_mask(code, 16, 19);
    uint32_t Rn = process_mask(code, 12, 15);
    uint32_t Rs = process_mask(code, 8, 11);
    uint32_t Rm = process_mask(code, 0, 3);

    uint32_t result = cpu_state->registers[Rm] * cpu_state->registers[Rs];
    if (accumulate){
        result += cpu_state->registers[Rn];
    }

    if (set){
        set_CPSR_flag(cpu_state, N, bit_mask(result, 31));
        if (!result){
            set_CPSR_flag(cpu_state, Z, 1);
        }
    }

    cpu_state->registers[Rd] = result;

}
