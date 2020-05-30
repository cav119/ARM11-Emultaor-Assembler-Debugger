#ifndef ASM_TYPES_H 
#define PIPELINE_DATA_H

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

// Label and the instruction it points to
typedef struct {
   char *name;
   // The line of the instruction that it points to
   uint32_t defined_instr_line; 
} Label;


typedef struct {
    char *name;
    uint32_t *instruction;
    // the line number of the branch instruction 
    uint32_t instruction_line;

} WaitingLabel;

#endif

