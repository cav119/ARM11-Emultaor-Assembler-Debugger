#include "asm_branch_instr.h"

Instruction *decode_branch_instr_to_bin(char code[5][512]){
    char[512] rule = code[1];
    if (cond[0] != 'b'){
        // must be a label, not a jump
    }
    else {
        // get the condition
        char cond[2] = {cond[0][1], cond[0][2], '\0'}; 
    }
    return 0;
}
