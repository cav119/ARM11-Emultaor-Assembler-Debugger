#include "asm_multiply_instr.h"
#include "asm_utilities.h"

#include <stdint.h>

AsmInstruction *encode_multiply(char *instr[], long *instr_line){
    AsmInstruction *mult = calloc(1, sizeof(AsmInstruction));
    uint32_t *n = malloc(sizeof(uint32_t));
    *n = (111 << 29);
    *n = setBit(*n, 4);
    *n = setBit(*n, 7);
    *n |= (read_red_num(instr[1]) << 16);
    *n|= read_red_num(instr[2]);
    *n|= (read_red_num(instr[3]) << 8);

    if (strcasecmp(instr[0], "mul") != 0){
        *n = setBit(*n, 21);
        *n|= (read_red_num(instr[4]) << 12);
    }
    mult->code = n;
    mult->instr_line = *instr_line;
    return mult;
}

