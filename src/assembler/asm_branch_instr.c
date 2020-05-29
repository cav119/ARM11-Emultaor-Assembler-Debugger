#include "asm_branch_instr.h"

Instruction *decode_branch_instr_to_bin(char code[5][512]){
    char *rule = code[1];
    if (rule[0] != 'b'){
        // must be a label, not a jump
    }
    else {
        // get the condition
        char cond[2] = {code[0][1], code[0][2], '\0'};
    }
    return 0;
}
