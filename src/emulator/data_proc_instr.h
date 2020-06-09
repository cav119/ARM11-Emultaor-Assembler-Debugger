#ifndef DATA_PROC_INSTR_H
#define DATA_PROC_INSTR_H
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include "cpu_state.h"
#include "pipeline_data.h"
#include "utilities.h"
#include "barrel_shifter.h"

// Opcode enum
typedef enum {
	AND = 0,
	EOR = 1,
	SUB = 2,
	RSB = 3,
	ADD = 4,
	TST = 8,
	TEQ = 9,
	CMP = 10,
	ORR = 12,
	MOV = 13
} opcode_type;


// Executes a data processing type instruction
void execute_data_processing_instr(CpuState *cpu_state, Instruction *instr);

#endif
