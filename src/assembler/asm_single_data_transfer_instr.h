#ifndef ASM_SINGLE_DATA_TRANSFER_INSTR_H
#define  ASM_SINGLE_DATA_TRANSFER_INSTR_H

#include "list.h"

// Encodes an SDT instruction into its binary representation
// The lists passed in are needed in case there is an LDR instruction that
// needs to dump constant address data at the end of the file.
uint32_t encode_sdt_instr_to_binary(char *tokens[], uint32_t curr_instr, 
    List *dumped_bytes_list, List *pending_offset_instr_addrs);

#endif
