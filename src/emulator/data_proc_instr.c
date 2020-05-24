#include "data_proc_instr.h"

void execute_data_processing_instr(CpuState *cpu_state, Instruction *instr) {
    //Assertions/checks
    assert(instr->type = data_process);

    //Variable definitions
    uint8_t immediate_enable_bit = process_mask(instr->code, 25, 25);
    uint8_t opcode = process_mask(instr->code, 21, 24);
    uint8_t cpsr_enable_bit = process_mask(instr->code, 20, 20);
    uint32_t operand_1 = *(cpu_state->registers + process_mask(instr->code, 16, 19));
    uint8_t dest_register = process_mask(instr->code, 12, 15);
    uint32_t operand_2 = process_mask(instr->code, 0, 11);
    //result will be the computed result that is written into the dest_register
    uint32_t result = 0;
    //if write_result is 0, then the result is NOT written to the dest_register
    uint8_t write_result = 1;
    //c_bit is 1 if 1 is to be written to the C bit of CPSR
    uint8_t c_bit = 0;

    //Compute Operand2
    if (immediate_enable_bit) {
        operand_2 = process_mask(instr->code, 0, 7);
        operand_2 = rotate_right(operand_2, process_mask(instr->code, 8, 11) * 2);
    } else {
        uint32_t shift_amount = process_mask(instr->code, 7, 11);
        //get the contents of register specified by bits 0-3
        uint32_t reg_contents = *(cpu_state->registers + process_mask(instr->code, 0, 3));

        //If bit 4 is 1:
        if (process_mask(instr->code, 4, 4)) {
            switch(process_mask(instr->code, 6, 5)) {
                case lsl:
                    operand_2 = reg_contents << shift_amount;
                    //C bit of CPSR is set to bit 32 after shift operation (carry out)
                    c_bit = (((uint64_t) reg_contents) >> 32) & 1;
                    break;
                case lsr:
                    operand_2 = reg_contents >> shift_amount;
                    //bit (shift_amount - 1) is the carry out
                    c_bit = (operand_2 >> (shift_amount - 1)) & 1;
                    break;
                case asr: 
                    operand_2 = arithmetic_shift_right(reg_contents, shift_amount);
                    //bit (shift_amount - 1) is the carry out
                    c_bit = (operand_2 >> (shift_amount - 1)) & 1;
                    break;
                case ror:
                    operand_2 = rotate_right(reg_contents, shift_amount);
                    //bit (shift_amount - 1) is the carry out
                    c_bit = (operand_2 >> (shift_amount - 1)) & 1;
                    break;
                default:
                    //default should not be reached - error
                    exit(EXIT_FAILURE);
            }
        } else {
            //lower_byte of the register corresponding to bits 8-11
            uint8_t lower_byte = *(cpu_state->registers + process_mask(instr->code, 8, 11));
            switch(process_mask(instr->code, 6, 5)) {
                case lsl:
                    operand_2 = reg_contents << lower_byte; 
                    //C bit of CPSR is set to bit 32 after shift operation (carry out)
                    //c_bit = (1 << 32) >> lower_byte;
                    c_bit = (((uint64_t) reg_contents) >> 32) & 1;
                    break;
                case lsr:
                    operand_2 = reg_contents >> lower_byte; 
                    c_bit = (operand_2 >> (lower_byte - 1)) & 1;
                    break;
                case asr: 
                    operand_2 = arithmetic_shift_right(reg_contents, lower_byte);
                    c_bit = (operand_2 >> (lower_byte - 1)) & 1;
                    break;
                case ror:
                    operand_2 = rotate_right(reg_contents, lower_byte);
                    c_bit = (operand_2 >> (lower_byte - 1)) & 1;
                    break;
                default:
                    //default should not be reached - error
                    exit(EXIT_FAILURE);
            }
        }
    }

    //Execute
    switch (opcode) {
        case and:
            result = operand_1 & operand_2;	
            break;
        case eor:
            result = operand_1 ^ operand_2;
            break;
        case sub:
            result = operand_1 - operand_2;
            //set c_bit if operand_2 is bigger than operand_1 which means overflow
            c_bit = operand_2 > operand_1 ? 1 : 0;
            break;
        case rsb:
            result = operand_2 - operand_1;
            //set c_bit if overflow
            c_bit = operand_1 > operand_2 ? 1 : 0;
            break;
        case add:
            result = operand_1 + operand_2;
            //If operand_1 + operand_2 overflows, set c_bit = 1
            uint64_t overflow_check = operand_1 + operand_2;
            c_bit = overflow_check >= (((uint64_t) 1) << 32) ? 1 : 0;
            break;
        case tst:
            result = operand_1 & operand_2;
            write_result = 0;
            break;
        case teq:
            result = operand_1 ^ operand_2;
            write_result = 0;
            break;
        case cmp:
            result = operand_1 - operand_2;
            write_result = 0;
            //If operand_2 > operand_1, then there will be a overflow from the subtraction
            c_bit = operand_2 > operand_1 ? 1 : 0;
            break;
        case orr:
            result = operand_1 | operand_2;
            break;
        case mov:
            result = operand_2;
            break;
        default:
            //Opcode should not enter this default area - means error
            exit(EXIT_FAILURE);
    }

    //Write to dest_register
    if (write_result) {
        *(cpu_state->registers + dest_register) = result;
    }

    //CPSR FLAGS
    if (cpsr_enable_bit) {
        //V bit unaffected
    
        //C bit (bit 29 of CPSR) - set to c_bit which is determined by the opcode:
        *(cpu_state->registers + CPSR) = *(cpu_state->registers + CPSR) | ((c_bit & 1) << 29);

        //Z bit (bit 30 of CPSR) - Z is 1 iff result == 0:
        if (result == 0) {
            *(cpu_state->registers + CPSR) = *(cpu_state->registers + CPSR) | (1 << 30);
        } else {
            *(cpu_state->registers + CPSR) = *(cpu_state->registers + CPSR) ^ (*(cpu_state->registers + CPSR) & (1 << 30));
        }

        //N bit (bit 31 of CPSR) - set to bit 31 of result:
        *(cpu_state->registers + CPSR) = *(cpu_state->registers + CPSR) | ((1 << 31) & result);
    }
}

//Performs rotate right on operand by rotate_amount times and returns the result
uint32_t rotate_right(uint32_t operand, uint32_t rotate_amount) {
    uint32_t result;
    result = operand >> rotate_amount;
    //loop the least significant bits round to the most significant:
    result = result | (operand << (31 - rotate_amount));
    return result;
}

//Performs arithmetic shift right on operand by shift_amount times and returns the result
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
