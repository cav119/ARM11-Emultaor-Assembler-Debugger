#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>
#include <string.h>
#include "file_io.h"
#include "type_defs.h"




// // change return types

// // Creates a symbol table from the assembly file
// SymbolTable *build_symbol_table(const char **lines);

// // Generates the binary code for each assembly instruction
void generate_binary_code(const char **lines, int num_lines, /* SymbolTable table,*/ FILE *outf);

// // Parse each token from a line (instruction)
Instruction *decode_instruction(const char *instr[]);


#endif
