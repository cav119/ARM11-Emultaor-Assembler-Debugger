#ifndef SINGLE_DATA_TRANSFER_INSTR_H
#define SINGLE_DATA_TRANSFER_INSTR_H
#include <stdbool.h>
#include "cpu_state.h"
#include "pipeline_data.h"

// Executes a Single Data Transfer type instruction
void execute_single_data_transfer_instr(CpuState *cpu_state, Instruction *instr);

#endif
