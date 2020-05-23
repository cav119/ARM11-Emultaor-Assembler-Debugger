#include <stdint.h>
#include <assert.h>
#include "single_data_transfer.h"
#include "utilities.h"

#define IMMEDIATE_BIT process_mask(instr->code, 25, 25)
#define INDEXING_BIT process_mask(instr->code, 24, 24)
#define UP_BIT process_mask(instr->code, 23, 23)
#define TRANSFER_TYPE_BIT process_mask(instr->code, 20, 20)
#define OFFSET_BITS process_mask(instr->code, 0, 11)
#define BASE_REG_BITS process_mask(instr->code, 16, 19)
#define TRANSFER_REG_BITS process_mask(instr->code, 12, 15)


void single_data_transfer_instr_execute(CpuState *cpu_state, Instruction *instr) {
    // Need to check this, don't really understand:
    // if (cpu_state->registers[process_mask(instr->code, 16, 19) == cpu_state->registers[PC]) {
    //     assert(cpu_state->registers[PC] == cpu_state->memory[cpu_state->registers[PC]] + 8);
    // }

    uint16_t offset = compute_offset(instr);
    uint32_t address = compute_address(cpu_state, instr, offset);

    // if L is set, then we load into the register, otherwise store
    if (TRANSFER_TYPE_BIT) {
        cpu_state->registers[TRANSFER_REG_BITS] = cpu_state->memory[address];
    } else {
        cpu_state->memory[address] = cpu_state->registers[TRANSFER_REG_BITS];
    }

}

uint16_t compute_offset(Instruction *instr) {
    uint16_t offset;
    if (IMMEDIATE_BIT) {
        // offset = 
    } else {
        offset = OFFSET_BITS;
    }

    return offset;
}

uint32_t compute_address(CpuState *cpu_state, Instruction *instr, uint16_t offset) {
    uint32_t address;
    uint32_t base_reg_val = cpu_state->registers[BASE_REG_BITS];

    if (UP_BIT) {                                               // up bit is set
        if (INDEXING_BIT) {                                         // pre-indexing
            address = base_reg_val + compute_offset(instr);
        } else {                                                    // post-indexing
            address = base_reg_val;
            cpu_state->registers[BASE_REG_BITS] += offset; 
        }
    } else {                                                    // up bit not set
        if (INDEXING_BIT) {                                         // pre-indexing
            address = base_reg_val - compute_offset(instr) ;
        } else {                                                    // post-indexing
            address = base_reg_val;
            cpu_state->registers[BASE_REG_BITS] -= offset; 
        }
    }

    return address;
}
