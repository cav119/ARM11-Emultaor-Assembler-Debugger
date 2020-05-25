#ifndef BRANCH_INSTR_H
#define BRANCH_INSTR_H
#include <stdbool.h>
#include "pipeline_data.h"

void execute_branch_instr(Instruction *instr, CpuState *cpu_state);


#endif

