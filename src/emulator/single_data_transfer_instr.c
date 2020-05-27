#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "single_data_transfer_instr.h"
#include "utilities.h"
#include "barrel_shifter.h"

#define IMMEDIATE_BIT process_mask(instr->code, 25, 25)
#define INDEXING_BIT process_mask(instr->code, 24, 24)
#define UP_BIT process_mask(instr->code, 23, 23)
#define TRANSFER_TYPE_BIT process_mask(instr->code, 20, 20)
#define OFFSET_BITS process_mask(instr->code, 0, 11)
#define BASE_REG_BITS process_mask(instr->code, 16, 19)
#define TRANSFER_REG_BITS process_mask(instr->code, 12, 15)


void execute_single_data_transfer_instr(CpuState *cpu_state, Instruction *instr) {
    assert(TRANSFER_REG_BITS < NUM_REGISTERS);

    uint16_t offset = compute_offset(cpu_state, instr);
    uint32_t address = compute_address(cpu_state, instr, offset);

    bool valid = check_valid_memory_access(cpu_state, address);
    if (!valid) {
        return;
    }

    // Load/store based on the transfer operation bit, rearranging for little endian
    if (TRANSFER_TYPE_BIT) {
        uint8_t b1 = cpu_state->memory[address];
        uint8_t b2 = cpu_state->memory[address + 1];
        uint8_t b3 = cpu_state->memory[address + 2];
        uint8_t b4 = cpu_state->memory[address + 3];
        cpu_state->registers[TRANSFER_REG_BITS] = b1 | (b2 << 8) | (b3 << 16) | (b4 << 24);
    } else {
        uint32_t reg_val = cpu_state->registers[TRANSFER_REG_BITS];
        cpu_state->memory[address] = process_mask(reg_val, 0, 7);
        cpu_state->memory[address + 1] = process_mask(reg_val, 8, 15);
        cpu_state->memory[address + 2] = process_mask(reg_val, 16, 23);
        cpu_state->memory[address + 3] = process_mask(reg_val, 24, 31);
    }
}


uint16_t compute_offset(CpuState *cpu_state, Instruction *instr) {
    uint16_t offset;
    if (IMMEDIATE_BIT) {
        // Register shifted offset (as in data processing type instructions)
        uint8_t carry = 0;
        offset = reg_offset_shift(cpu_state, instr, &carry);
    } else {
        offset = OFFSET_BITS;
    }

    return offset;
}


uint32_t compute_address(CpuState *cpu_state, Instruction *instr, uint16_t offset) {
    uint32_t address;
    uint32_t base_reg_val = cpu_state->registers[BASE_REG_BITS];

    if (UP_BIT) {                                          // up bit is set
        if (INDEXING_BIT) {                                    // pre-indexing
            address = base_reg_val + offset;
        } else {                                               // post-indexing
            address = base_reg_val;
            cpu_state->registers[BASE_REG_BITS] += offset; 
        }
    } else {                                               // up bit not set
        if (INDEXING_BIT) {                                    // pre-indexing
            address = base_reg_val - offset;
        } else {                                               // post-indexing
            address = base_reg_val;
            cpu_state->registers[BASE_REG_BITS] -= offset; 
        }
    }

    return address;
}
