#ifndef ASM_DATA_PROC_INSTR_H
#define ASM_DATA_PROC_INSTR_H
#include <stdint.h>
#include "type_defs.h"

//Takes in the data proc instruction as an array of strings (split up)
//Size is the number of split strings in the array (needed for optional shift
//register scenario) - set size to 3 to skip the shift register option
//Returns corresponding binary instruction
AsmInstruction *encode_dp_instr_to_binary(char *instr[], uint8_t size, long *instr_line);

#endif
