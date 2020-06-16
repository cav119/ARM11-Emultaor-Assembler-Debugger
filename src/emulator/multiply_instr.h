#ifndef MULTIPLY_INSTR_H
#define MULTIPLY_INSTR_H
#include "pipeline_data.h"
#include <stdbool.h>
#include "cpu_state.h"

// Executes a multiply type instruction
void execute_multiply_instruction(Instruction *instruction, CpuState *cpuState);

#endif
