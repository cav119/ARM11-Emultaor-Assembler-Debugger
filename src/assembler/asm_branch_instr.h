#ifndef ASM_BRANCH_H
#define ASM_BRANCH_H

#include <stdint.h>
#include <stdbool.h>

#include "asm_utilities.h"
#include "type_defs.h"
#include "hash_table.h"

// returns a pointer to an Instruction that forms from 
// reading the 5 strings from the code
uint32_t *decode_branch_instr_to_bin(char code[5][512], HashTable *symbol_table
        , WaitingBranchInstr  **waiting_labels, int *waiting_br_size, int current_line, bool *label_next_instr, char *waiting_label);

#endif
