#ifndef PIPELINE_DATA_H
#define PIPELINE_DATA_H

#include "../../extension/command_parser.h"

// instruction_type enum
typedef enum {
	DATA_PROCESS,
	MULTIPLY,
	SINGLE_DATA_TRANSFER,
	BRANCH
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

