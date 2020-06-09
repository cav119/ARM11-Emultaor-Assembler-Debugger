#ifndef BARREL_SHIFTER_H
#define BARREL_SHIFTER_H
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cpu_state.h"
#include "pipeline_data.h"
#include "utilities.h"

// shift_op enum is used to specify which shift operation
typedef enum {
	LSL,
	LSR,
	ASR,
	ROR
} shift_op;


// Shift operation on operand by shift_amount, according to shift_opcode
// Sets the carry to c_bit 
uint32_t execute_shift(uint32_t operand, uint32_t shift_amount, uint8_t shift_opcode, 
	uint8_t *c_bit);

// Using offset from instruction, shifts register according to the spec and
// returns the result (used by data_process_instr and data_transfer_instr)
uint32_t reg_offset_shift(CpuState *cpu_state, Instruction *instr, uint8_t *c_bit);

// Rotate right on operand by rotate_amount times and returns result
uint32_t rotate_right(uint32_t operand, uint32_t rotate_amount);

// Arithmetic shift right on operand by shift_amount times and returns result
uint32_t arithmetic_shift_right(uint32_t operand, uint32_t shift_amount);

#endif
