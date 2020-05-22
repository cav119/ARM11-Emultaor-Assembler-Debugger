#ifndef PIPELINE_H
#define PIPELINE_H

// instruction_type enum
typedef enum {
	data_process,
	multiply,
	single_data_transfer,
	branch
} instruction_type;

// Instruction struct
typedef struct {
	uint32_t code;
	instruction_type type;	
} Instruction;

// Starts the fetch-decode-execute process until finished
void start_pipeline(CpuState *cpu_state);

// Decodes the 4 byte instruction and returns a pointer to Instruction struct
Instruction *decode_instruction(uint32_t bits);

#endif