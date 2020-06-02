#ifndef PARSER_H
#define PARSER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "type_defs.h"
#include "hash_table.h"



// Transforms a line into 5 tokens
char **instr_to_tokens(char array[]);

// Encodes an instruction
uint32_t *decode_instruction(const char *instr[], long *instr_number,
                                HashTable *symbol_table, HashTable *waiting_branches, bool *label_next_instr);

void encode_file_lines(char **lines, size_t nlines);

#endif
