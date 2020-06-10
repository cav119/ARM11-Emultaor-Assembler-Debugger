#ifndef ASM_SINGLE_DATA_TRANSFER_INSTR_H
#define  ASM_SINGLE_DATA_TRANSFER_INSTR_H

#include "typedefs.h"
// Encodes an SDT instruction into its binary representation
AsmInstruction *encode_sdt_instr_to_binary(char *tokens[], long *instr_line);

#endif
