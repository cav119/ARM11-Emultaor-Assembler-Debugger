#ifndef BRANCH_INSTR_H
#define BRANCH_INSTR_H
#include <stdbool.h>
#include "pipeline_data.h"

// Executes a branch type instruction
void execute_branch_instr(Instruction *instr, CpuState *cpu_state, Pipe* pipe);

#endif

