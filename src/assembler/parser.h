#ifndef PARSER_H
#define PARSER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "array_list.h"
#include "type_defs.h"
#include "hash_table.h"
#include "list.h"


// Transforms a line into 5 tokens
char **instr_to_tokens(char array[]);

// Encodes an instruction
void decode_instruction(const char *instr[], long *instr_number,
    HashTable *symbol_table, ArrayList *waiting_branches,
    bool *label_next_instr, ArrayList *waiting_label, List *instructions,
    List *dumped_bytes, List *pending_offset_addrs);

// One-pass assembler function (called from main)
void one_pass_assemble(FILE *inp_file, FILE *out_file);

#endif
