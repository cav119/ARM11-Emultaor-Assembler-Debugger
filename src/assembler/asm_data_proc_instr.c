#include "asm_data_proc_instr.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

//Private helper function declarations:
uint16_t compute_operand_2(const char *expression[], uint8_t *i_bit, uint8_t size); 
uint16_t compute_imm_expression(const char *expression); 
uint16_t compute_operand_2_shift(const char *expression[]);
uint8_t get_data_proc_opcode(char *op);
uint16_t read_reg_num(char *reg);
uint32_t rotate_left(uint32_t operand, uint32_t rotate_amt);
uint8_t get_shift_opcode(char *op);
uint32_t read_imm_val(const char *expression);

/*Takes in the data proc instruction as an array of strings (split up)
Size is the number of tokens in instr[] (needed for optional shift
register scenario) - set size to 3 to skip the shift register option
Returns corresponding binary instruction in big endian format*/
AsmInstruction *encode_dp_instr_to_binary(char *instr[], uint8_t size, long *instr_line) {
    assert(size >= 3);

    AsmInstruction *inst = calloc(1, sizeof(AsmInstruction));
    uint32_t *code = malloc(sizeof(uint32_t));
    inst->instr_line = *instr_line;
    inst->code = code;

    //Check for special instruction "andeq r0, r0, r0" (halt instruction)
    if (strcmp(instr[0], "andeq") == 0
        && strcmp(instr[1], "r0") == 0
        && strcmp(instr[2], "r0") == 0
        && strcmp(instr[3], "r0") == 0) {
        *code = 0;
        return inst; 
    }

    //Special instruction "lsl Rn, <#expression>" - manually changes instr
    if (strcmp(instr[0], "lsl") == 0) {
        char *new_instr[] = {"mov", instr[1], instr[1], "lsl", instr[2], NULL};
        free(code);
        free(inst);
        return encode_dp_instr_to_binary(new_instr, 5, instr_line);
    }

    //instruction parts
    uint8_t cond = 0x0E;
    uint8_t i_bit = 0;
    uint8_t opcode = get_data_proc_opcode(instr[0]); 
    uint8_t s_bit = 0;
    uint8_t reg_rn = 0;
    uint8_t reg_rd = 0;
    uint16_t operand_2 = 0;
    //i is the encoded instruction returned
    uint32_t i = 0;

    //build parts, according to opcode type:
    if ((0 <= opcode && opcode <= 4) || opcode == 12) {
        reg_rd = read_reg_num(instr[1]);
        reg_rn = read_reg_num(instr[2]);
        operand_2 = compute_operand_2((const char **) instr + 3, &i_bit, size - 3);
    } else if (opcode == 13) { //mov
        reg_rd = read_reg_num(instr[1]);
        operand_2 = compute_operand_2((const char **) instr + 2, &i_bit, size - 2);
    } else if (8 <= opcode && opcode <= 10) { //tst, teq, cmp
        s_bit = 1;
        reg_rn = read_reg_num(instr[1]);
        operand_2 = compute_operand_2((const char **) instr + 2, &i_bit, size - 2);
    } else {
        printf("Error: instruction has unidentified opcode");
        exit(EXIT_FAILURE);
    }

    //build i using parts
    i |= (cond << 28);
    i = i_bit ? i | (1 << 25) : i & ~(1 << 25);
    i |= ((opcode & 0x0F) << 21);
    i = s_bit ? i | (1 << 20) : i & ~(1 << 20);
    i |= ((reg_rn & 0x0F) << 16);
    i |= ((reg_rd & 0x0F) << 12);
    i |= (operand_2 & 0x0FFF);
    *code = i;
    return inst;
}

/* Takes in the strings which are used for computing operand_2
Returns operand_2 as a uint16_t
Sets i_bit to 1 if an immediate expression, and 0 if not
If size is <= 1, then the optional register shift is skipped.*/
uint16_t compute_operand_2(const char *expression[], uint8_t *i_bit, uint8_t size) {
    if (expression[0][0] == '#') {
        *i_bit = 1;
        return compute_imm_expression(expression[0]);
    } else {
        *i_bit = 0;
        if (size <= 1) {
            //If only a register (no shift) is specified
            return read_reg_num((char *) expression[0]);
        }
        return compute_operand_2_shift(expression);
    }
}

//Computes operand_2 that gets the corresponding expression value
//Assumes that imm value is <= 0xFFFFFFFF (otherwise overflow occurs)
uint16_t compute_imm_expression(const char *expression) {
    uint32_t imm_val = read_imm_val(expression);
    assert(imm_val <= 0xFF000000);

    for (int i = 0; i < 32; i += 2) {
       if ((rotate_left(imm_val, i) >> 8) == 0) {
           return (rotate_left(imm_val, i) & 0xFF) | ((i/2) << 8);
       }
    }

    //If the value cannot be represented, then give an error as the emulator
    printf("The immediate value supplied in #expression cannot be used.");
    exit(EXIT_FAILURE);
}

//converts a string to the corresponding number (can be in hex or base 10)
uint32_t read_imm_val(const char *expression) {
    assert(expression[0] == '#');

    if (strlen(expression) >= 3 && expression[2] == 'x') {
        return (int) strtol(strtok((char *) expression, "#"), NULL, 16);
    } else {
        return atoi(strtok((char *) expression, "#")); 
    }   
}

//Computes operand_2 to perform a shift according to expression's strings
uint16_t compute_operand_2_shift(const char *expression[]) {
    uint8_t reg_m = read_reg_num((char *) expression[0]);
    uint8_t shift_opcode = get_shift_opcode((char *) expression[1]);
    //Check if third arg is an immediate expression
    if (expression[2][0] == '#') {
        uint8_t imm_val = read_imm_val(expression[2]);
        return (imm_val << 7) | (shift_opcode << 5) | reg_m;
    } else {
        uint8_t reg_s = read_reg_num((char *) expression[2]);
        return (reg_s << 8) | 0x10 | (shift_opcode << 5) | reg_m;
    }
}

//gets corresponding opcode for shift type that string op specifies
uint8_t get_shift_opcode(char *op) {
    if (strcmp(op, "lsl") == 0) {
        return 0;
    } else if (strcmp(op, "lsr") == 0) {
        return 1;
    } else if (strcmp(op, "asr") == 0) {
        return 2;
    } else if (strcmp(op, "ror") == 0) {
        return 3;
    } else {
        perror("Error: Supplied shift op string is unidentifiable.\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}

//Returns matching opcode from op string
uint8_t get_data_proc_opcode(char *op) {
    if (strcmp(op, "and") == 0) {
        return 0;
    } else if (strcmp(op, "eor") == 0) {
        return 1;
    } else if (strcmp(op, "sub") == 0) {
        return 2;
    } else if (strcmp(op, "rsb") == 0) {
        return 3;
    } else if (strcmp(op, "add") == 0) {
        return 4;
    } else if (strcmp(op, "tst") == 0) {
        return 8;
    } else if (strcmp(op, "teq") == 0) {
        return 9;
    } else if (strcmp(op, "cmp") == 0) {
        return 10;
    } else if (strcmp(op, "orr") == 0) {
        return 12;
    } else if (strcmp(op, "mov") == 0) {
        return 13;
    } else {
        perror("Error: Supplied data proc op string is unidentifiable.\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}

//read_reg_num is temporary function using read_red_num from assemble.c
uint16_t read_reg_num(char *reg) {
    if (reg == 0) {
        return 0;
    }
    return atoi(strtok(reg, "r"));
}

//Rotates left operand by rotate_amt
uint32_t rotate_left(uint32_t operand, uint32_t rotate_amt) {
    return (operand << rotate_amt) | (operand >> (32 - rotate_amt));
}
