#ifndef ASM_TYPES_H
#define ASM_TYPES_H
#include <stdint.h>
#include <stdbool.h>


// instruction_type enum
typedef enum {
	data_process,
	multiply,
	single_data_transfer,
	branch,
  special
} instruction_type;

// Instruction struct
typedef struct {
	uint32_t code;
	instruction_type type;	
} Instruction;

typedef struct {
    char *name;
    bool solved;
} WaitingLabel;

// Assembler full instruction 
typedef struct {
    uint32_t *code;
    uint32_t instr_line;
} AsmInstruction;

typedef struct {
    char *name;
    uint32_t *instruction;
    long instr_line; // the line number of the branch instruction 
    bool solved;
} WaitingBranchInstr;


#endif
