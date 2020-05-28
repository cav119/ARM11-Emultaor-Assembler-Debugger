#ifndef SINGLE_DATA_TRANSFER_INSTR_H
#define SINGLE_DATA_TRANSFER_INSTR_H
#include <stdbool.h>
#include "cpu_state.h"
#include "pipeline_data.h"

// Executes a Single Data Transfer type instruction
void execute_single_data_transfer_instr(CpuState *cpu_state, Instruction *instr);

// Computes the offset for the address
uint16_t compute_offset(CpuState *cpu_state, Instruction *instr);

// Computes the memory address to be used for the transfer given the offset
uint32_t compute_address(CpuState *cpu_state, Instruction *instr, uint16_t offset);

#endif
