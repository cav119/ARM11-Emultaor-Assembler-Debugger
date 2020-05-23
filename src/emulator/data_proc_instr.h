#ifndef DATA_PROC_INSTR_H
#define DATA_PROC_INSTR_H
#include "cpu_state.h"
#include "pipeline.h"
#include "utilities.h"
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

void execute_data_processing_instr(CpuState *cpu_state, Instruction *instr);

#endif
