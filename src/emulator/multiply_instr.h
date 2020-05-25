#ifndef MULTIPLY_INSTR_H
#define MULTIPLY_INSTR_H
#include "pipeline_data.h"
#include <stdbool.h>

void execute_multiply_instruction(Instruction *instruction, CpuState *cpuState);

#endif
