#ifndef PARSER_H
#define PARSER_H
#include "type_defs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Transforms a line into 5 tokens
char **instr_to_tokens(char array[]);

// Encodes an instruction
Instruction *decode_instruction(const char *instr[]);

#endif
