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


#endif

