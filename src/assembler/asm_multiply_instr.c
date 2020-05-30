#include "asm_multiply_instr.h"

#include <stdint.h>

Instruction *encode_multiply(char *instr[]){
    uint32_t n = (111 << 29);
    n = setBit(n, 4);
    n = setBit(n, 7);
    n |= (read_red_num(instr[1]) << 16);
    n|= read_red_num(instr[2]);
    n|= (read_red_num(instr[3]) << 8);

    if (strcasecmp(instr[0], "mull") != 0){
        n = setBit(n, 21);
        n|= (read_red_num(instr[4]) << 12);
    }
    Instruction *instruction;
    instruction->type = multiply;
    instruction->code = n;
return instruction;
}