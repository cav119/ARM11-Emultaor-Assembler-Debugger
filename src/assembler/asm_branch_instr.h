#include "asm_utilities.h"
#include "type_defs.h"

// returns a pointer to an Instruction that forms from 
// reading the 5 strings from the code
Instruction *decode_branch_instr_to_bin(char code[5][512]);
