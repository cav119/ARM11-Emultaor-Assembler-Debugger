#ifndef ASM_BRANCH_H
#define ASM_BRANCH_H
#include "asm_utilities.h"
#include "type_defs.h"
#include "hash_table.h"

// returns a pointer to an Instruction that forms from 
// reading the 5 strings from the code
Instruction *decode_branch_instr_to_bin(char code[5][512], HashTable *symbol_table
        , HashTable *waiting_labels, int current_line);

#endif
