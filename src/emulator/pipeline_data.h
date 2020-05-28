#ifndef PIPELINE_DATA_H
#define PIPELINE_DATA_H

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

// Pipeline struct
typedef struct {
	Instruction *executing;
	Instruction *decoding;
	uint32_t fetching;
} Pipe;

#endif

