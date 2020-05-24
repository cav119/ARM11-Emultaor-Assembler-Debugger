#include "barrel_shifter.h"

//Shift operation on operand by shift_amount, according to shift_opcode
//Sets the carry to c_bit 
uint32_t execute_shift(uint32_t operand, uint32_t shift_amount, 
        uint8_t shift_opcode, uint8_t *c_bit) {
    uint32_t result;

    switch (shift_opcode) {
        case lsl:
            result = operand << shift_amount;
            //CPSR c bit set to bit 32 after lsl (carry out)
            *c_bit = (((uint64_t) operand) >> 32) & 1;
            break;
        case lsr:
            result = operand >> shift_amount;
            //bit (shift_amount - 1) is the carry out
            *c_bit = (result >> (shift_amount - 1)) & 1;
            break;
        case asr: 
            result = 
            arithmetic_shift_right(operand, shift_amount);
            //bit (shift_amount - 1) is the carry out
            *c_bit = (result >> (shift_amount - 1)) & 1;
            break;
        case ror:
            result = rotate_right(operand, shift_amount);
            //bit (shift_amount - 1) is the carry out
            *c_bit = (result >> (shift_amount - 1)) & 1;
            break;
        default:
            //default should not be reached - error
            exit(EXIT_FAILURE);
    }

    return result;
}

//Using offset from instruction, shifts register according to the spec and
//returns the result (used by data_process_instr and data_transfer_instr)
uint32_t reg_offset_shift(CpuState *cpu_state, Instruction *instr, 
        uint8_t *c_bit) {
    uint32_t result;

    //get the contents of register specified by bits 0-3
    uint32_t reg_contents = 
        *(cpu_state->registers + process_mask(instr->code, 0, 3));
    uint8_t shift_op = process_mask(instr->code, 5, 6);

    //If bit 4 is 1:
    if (process_mask(instr->code, 4, 4)) {
        uint32_t shift_amount = process_mask(instr->code, 7, 11);
        result = 
            execute_shift(reg_contents, shift_amount, shift_op, &c_bit);
    } else {
        //lower_byte of the register corresponding to bits 8-11
        uint8_t lower_byte = 
            *(cpu_state->registers + process_mask(instr->code, 8, 11));
        result = 
            execute_shift(reg_contents, lower_byte, shift_op, &c_bit);
    }

    return result;
}



//Rotate right on operand by rotate_amount times and returns result
uint32_t rotate_right(uint32_t operand, uint32_t rotate_amount) {
    uint32_t result;
    result = operand >> rotate_amount;
    //loop the least significant bits round to the most significant:
    result = result | (operand << (31 - rotate_amount));
    return result;
}

//Arithmetic shift right on operand by shift_amount times and returns result
uint32_t arithmetic_shift_right(uint32_t operand, uint32_t shift_amount) {
    uint32_t result;
    //If MSB of operand is 1
    if ((1 << 31) & operand) {
        result = operand >> shift_amount;
        //sign extension in for loop
        for (int i = 0; i < shift_amount; i++) {
            result = result | (1 << (31 - i));
        }
    } else {
        result = operand >> shift_amount;
    }
    return result;
}
