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
    // Need to check this, don't really understand:
    // if (cpu_state->registers[process_mask(instr->code, 16, 19) == cpu_state->registers[PC]) {
    //     assert(cpu_state->registers[PC] == cpu_state->memory[cpu_state->registers[PC]] + 8);
    // }

    uint16_t offset = compute_offset(cpu_state, instr);
    uint32_t address = compute_address(cpu_state, instr, offset);

    assert(TRANSFER_REG_BITS < NUM_REGISTERS);
    // if L is set, then we load into the register, otherwise store
    if (TRANSFER_TYPE_BIT) {
        uint8_t b1 = cpu_state->memory[address];
        uint8_t b2 = cpu_state->memory[address + 1];
        uint8_t b3 = cpu_state->memory[address + 2];
        uint8_t b4 = cpu_state->memory[address + 3];
        // Rearrangement for little endian
        cpu_state->registers[TRANSFER_REG_BITS] = b1 | (b2 << 8) | (b3 << 16) | (b4 << 24);
    } else {
        uint32_t reg_val = cpu_state->registers[TRANSFER_REG_BITS];
        // Store 4 byte word into four 1-byte memory locations
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
        uint8_t *carry = 0;
        offset = reg_offset_shift(cpu_state, instr, carry);
    } else {
        offset = OFFSET_BITS;
    }
    // printf("Offset: %d\n", offset);
    return offset;
}

uint32_t compute_address(CpuState *cpu_state, Instruction *instr, uint16_t offset) {
    uint32_t address;
    uint32_t base_reg_val = cpu_state->registers[BASE_REG_BITS];

    if (UP_BIT) {                                               // up bit is set
        if (INDEXING_BIT) {                                         // pre-indexing
            address = base_reg_val + offset;
        } else {                                                    // post-indexing
            address = base_reg_val;
            cpu_state->registers[BASE_REG_BITS] += offset; 
        }
    } else {                                                    // up bit not set
        if (INDEXING_BIT) {                                         // pre-indexing
            address = base_reg_val - offset;
        } else {                                                    // post-indexing
            address = base_reg_val;
            cpu_state->registers[BASE_REG_BITS] -= offset; 
        }
    }
    // printf("Address: %d (0x%x)\n", address, address);
    // printf("Up? %d \t Index? %s\n", UP_BIT, INDEXING_BIT ? "pre" : "post");
    return address;
}

// test single data instruction
// void test_sdi(CpuState *cpu_state) {
// 	// load sample random memory (first 50 bytes)
// 	srand(95959);
// 	for (int i = 0; i < 21; i++) {
// 		cpu_state->memory[i] = (rand() % 20);
// 	}

//     // TODO: TEST FOR THE NEW 4 BYTE LOADING/STORING (populate regs and memory with large vals)

// 	// load some register data for testing
// 	cpu_state->registers[1] = 3;
// 	cpu_state->registers[4] = 0x12345678;

// 	// print initial state
// 	print_registers(cpu_state);
// 	print_memory(cpu_state, 0, 30);

// 	Instruction *instr = malloc(sizeof(Instruction));
// 	 //0b0000 01 0 0 0 00 0 0001 0100 0000 0000 0001
// 	 //          I P U    S b=r1 d=r4   ofst = 1
// 	instr->code = 0x4014001;
// 	instr->type = single_data_transfer;

// 	// execute
// 	single_data_transfer_instr_execute(cpu_state, instr);

// 	// print state
// 	print_registers(cpu_state);
// 	print_memory(cpu_state, 0, 30);

// 	free(instr);
// }
