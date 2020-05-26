#ifndef PIPELINE_H
#define PIPELINE_H
#include <stdbool.h>
#include "cpu_state.h"
#include "pipeline_data.h"

#include "multiply_instr.h"
#include "branch_instr.h"
#include "single_data_transfer_instr.h"
#include "data_proc_instr.h"

Pipe *init_pipeline(CpuState *cpu_state);

uint32_t fetch(uint32_t pointer, CpuState *cpuState);

bool execute(Instruction *instruction, CpuState *cpuState, Pipe* pipe);

// Starts the fetch-decode-execute process until finished
void start_pipeline(CpuState *cpu_state);

// possibly ends the pipeline, returns true if pipeline was indeed ended
// return value true can be caused by a branch instruction which forces code to continue
bool end_pipeline(Pipe *pipe, CpuState *cpuState);

// Decodes the 4 byte instruction and returns a pointer to Instruction struct
Instruction *decode_instruction(uint32_t bits);

// after executing a branch instruction cpu must clear pipeline
void clear_pipe(Pipe* pipe);

void print_pipeline(Pipe *pipe);

// after executing an instr the pipeline must move everything to the level above
// fetch -> decode, decode -> execute and the new fetched inst is put in fetch
void pipe_cycle_once(Pipe* pipe, uint32_t just_fetched);



#endif
