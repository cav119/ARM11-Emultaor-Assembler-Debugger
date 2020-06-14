#ifndef PIPELINE_H
#define PIPELINE_H
#include <stdbool.h>
#include "cpu_state.h"
#include "pipeline_data.h"
#include "multiply_instr.h"
#include "branch_instr.h"
#include "single_data_transfer_instr.h"
#include "data_proc_instr.h"
#include "../extension/break_list.h"

// Initialises and allocates memory for a Pipe struct
Pipe *init_pipeline(CpuState *cpu_state);

// Fetches the next instruction from memory from the given ptr address, returns a little endian number
uint32_t fetch(uint32_t ptr, CpuState *cpu_state);

// Fetches the next instruction from memory from the given ptr address, returns a big endian number
uint32_t fetch_big_endian(uint32_t ptr, CpuState *cpu_state);

// Decodes the 4 byte instruction and returns a pointer to Instruction struct
Instruction *decode_instruction(uint32_t bits);

// Executes the instruction in the pipeline given the instruction
bool execute(Instruction *instruction, CpuState *cpu_state, Pipe *pipe);

// Starts the fetch-decode-execute process until finished
void start_pipeline(CpuState *cpu_state, bool is_extenstion);

// Possibly ends the pipeline, returns true if pipeline was indeed ended
// Return value true can be caused by a branch instruction which forces code to continue
bool end_pipeline(Pipe *pipe, CpuState *cpu_state, bool is_extension, bool is_stepping, List *list);

// After executing a branch instruction cpu must clear pipeline
void clear_pipe(Pipe *pipe);

// Prints the currrent pipeline
void print_pipeline(Pipe *pipe);

#endif
