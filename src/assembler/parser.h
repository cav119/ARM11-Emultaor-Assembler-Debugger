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
                                int *waiting_br_size, bool *label_next_instr, char *waiting_label, List *instructions);

void encode_file_lines(FILE*fp);

#endif
