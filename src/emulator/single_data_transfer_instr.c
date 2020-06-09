#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "single_data_transfer_instr.h"
#include "utilities.h"
#include "barrel_shifter.h"

#define IMMEDIATE_BIT(bits) bit_mask(bits, 25)
#define INDEXING_BIT(bits) bit_mask(bits, 24)
#define UP_BIT(bits) bit_mask(bits, 23)
#define TRANSFER_TYPE_BIT(bits) bit_mask(bits, 20)
#define OFFSET_BITS(bits) process_mask(bits, 0, 11)
#define BASE_REG_BITS(bits) process_mask(bits, 16, 19)
#define TRANSFER_REG_BITS(bits) process_mask(bits, 12, 15)


// Internal function to compute the offset for the address of an SDT instruction
static uint16_t compute_offset(CpuState *cpu_state, Instruction *instr) {
    uint16_t offset;
    if (IMMEDIATE_BIT(instr->code)) {
        // Register shifted offset (as in data processing type instructions)
        uint8_t carry = 0;
        offset = reg_offset_shift(cpu_state, instr, &carry);
    } else {
        offset = OFFSET_BITS(instr->code);
    }

    return offset;
}


// Internal function to compute the address given an offset of an SDT instruction
static uint32_t compute_address(CpuState *cpu_state, Instruction *instr, uint16_t offset) {
    uint32_t address;
    uint32_t base_reg_val = cpu_state->registers[BASE_REG_BITS(instr->code)];

    if (UP_BIT(instr->code)) {                             // up bit is set
        if (INDEXING_BIT(instr->code)) {                       // pre-indexing
            address = base_reg_val + offset;
        } else {                                               // post-indexing
            address = base_reg_val;
            cpu_state->registers[BASE_REG_BITS(instr->code)] += offset; 
        }
    } else {                                               // up bit not set
        if (INDEXING_BIT(instr->code)) {                       // pre-indexing
            address = base_reg_val - offset;
        } else {                                               // post-indexing
            address = base_reg_val;
            cpu_state->registers[BASE_REG_BITS(instr->code)] -= offset; 
        }
    }

    return address;
}


void execute_single_data_transfer_instr(CpuState *cpu_state, Instruction *instr) {
    assert(TRANSFER_REG_BITS(instr->code) < NUM_REGISTERS);

    uint16_t offset = compute_offset(cpu_state, instr);
    uint32_t address = compute_address(cpu_state, instr, offset);

    bool valid = check_valid_memory_access(cpu_state, address);
    if (!valid) {
        return;
    }

    // Load/store based on the transfer operation bit, rearranging for little endian
    if (TRANSFER_TYPE_BIT(instr->code)) {
        uint32_t word = index_little_endian_bytes(&(cpu_state->memory[address]));
        cpu_state->registers[TRANSFER_REG_BITS(instr->code)] = word;
    } else {
        uint32_t reg_val = cpu_state->registers[TRANSFER_REG_BITS(instr->code)];
        cpu_state->memory[address] = process_mask(reg_val, 0, 7);
        cpu_state->memory[address + 1] = process_mask(reg_val, 8, 15);
        cpu_state->memory[address + 2] = process_mask(reg_val, 16, 23);
        cpu_state->memory[address + 3] = process_mask(reg_val, 24, 31);
    }
}
