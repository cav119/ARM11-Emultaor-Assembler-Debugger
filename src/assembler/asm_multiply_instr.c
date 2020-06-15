#include "asm_multiply_instr.h"
#include "asm_utilities.h"

#include <stdint.h>
#include <strings.h>
#include <stdio.h>

AsmInstruction *encode_multiply(char *instr[], long *instr_line) {
    AsmInstruction *mult = calloc(1, sizeof(AsmInstruction));
    if (!mult) {
        perror("Error allocating memory for asm instruction");
        exit(EXIT_FAILURE);
    }
    uint32_t *n = malloc(sizeof(uint32_t));
    if (!n) {
        perror("Error allocating memory for code of asm instruction");
        exit(EXIT_FAILURE);
    }
    *n = (0xE << 28);   // condition code
    *n = set_bit(*n, 4);
    *n = set_bit(*n, 7);
    *n |= (read_reg_num(instr[1]) << 16);
    *n|= read_reg_num(instr[2]);
    *n|= (read_reg_num(instr[3]) << 8);

    if (strcasecmp(instr[0], "mul") != 0){
        *n = set_bit(*n, 21);
        *n|= (read_reg_num(instr[4]) << 12);
    }
    mult->code = n;
    mult->instr_line = *instr_line;
    return mult;
}

