#ifndef ASM_BRANCH_H
#define ASM_BRANCH_H

#include <stdint.h>
#include <stdbool.h>

#include "array_list.h"
#include "asm_utilities.h"
#include "type_defs.h"
#include "hash_table.h"

// returns a pointer to an Instruction that forms from reading the 5 strings from the code
AsmInstruction *encode_branch_instr(char **code, HashTable *symbol_table,
        ArrayList *waiting_branches, bool *label_next_instr,
        ArrayList *waiting_label, long *instr_line);

#endif
