#include "barrel_shifter.h"

#define SHIFTED_REG_M_BITS(bits) process_mask(bits, 0, 3)
#define SHIFT_TYPE_BITS(bits) process_mask(bits, 5, 6)
#define SHIFT_MODE_BIT(bits) bit_mask(bits, 4)
#define SHIFT_REGISTER_BITS(bits) process_mask(bits, 8, 11)
#define SHIFT_CONSTANT_BITS(bits) process_mask(bits, 7, 11)


uint32_t execute_shift(uint32_t operand, uint32_t shift_amount, uint8_t shift_opcode, 
    uint8_t *c_bit) {
    uint32_t result;

    switch (shift_opcode) {
        case LSL:
            result = operand << shift_amount;
            //CPSR c bit set to bit 32 after lsl (carry out)
            *c_bit = ((((uint64_t) operand) << shift_amount) >> 32) & 1;
            break;
        case LSR:
            result = operand >> shift_amount;
            //bit (shift_amount - 1) is the carry out
            *c_bit = (operand >> (shift_amount - 1)) & 1;
            break;
        case ASR: 
            result = arithmetic_shift_right(operand, shift_amount);
            //bit (shift_amount - 1) is the carry out
            *c_bit = (operand >> (shift_amount - 1)) & 1;
            break;
        case ROR:
            result = rotate_right(operand, shift_amount);
            //bit (shift_amount - 1) is the carry out
            *c_bit = (operand >> (shift_amount - 1)) & 1;
            break;
        default:
            //default should not be reached - error
            exit(EXIT_FAILURE);
    }

    return result;
}


uint32_t reg_offset_shift(CpuState *cpu_state, Instruction *instr, uint8_t *c_bit) {
    uint32_t result;

    //get the contents of register specified by bits 0-3
    uint32_t reg_contents = cpu_state->registers[SHIFTED_REG_M_BITS(instr->code)]; 

    //If bit 4 is 1:
    if (SHIFT_MODE_BIT(instr->code)) {
        uint8_t lower_byte = cpu_state->registers[SHIFT_REGISTER_BITS(instr->code)]; 
        result = execute_shift(reg_contents, lower_byte, SHIFT_TYPE_BITS(instr->code), c_bit);
    } else {
        result = execute_shift(reg_contents, SHIFT_CONSTANT_BITS(instr->code), 
            SHIFT_TYPE_BITS(instr->code), c_bit);              
    }

    return result;
}


uint32_t rotate_right(uint32_t operand, uint32_t rotate_amount) {
    uint32_t result = operand >> rotate_amount;
    //loop the least significant bits round to the most significant:
    result = result | (operand << (32 - rotate_amount));

    return result;
}


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
