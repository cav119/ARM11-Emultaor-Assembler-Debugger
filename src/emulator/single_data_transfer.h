#ifndef SINGLE_DATA_TRANSFER_INSTR_H
#define SINGLE_DATA_TRANSFER_INSTR_H

#include <stdbool.h>
#include "cpu_state.h"
#include "pipeline.h"

// Executes a Single Data Transfer type instruction
void single_data_transfer_instr_execute(CpuState *cpu_state, Instruction *instr);

// Computes the offset for the address
uint16_t compute_offset(Instruction *instr);

// Computes the memory address to be used for the transfer
uint32_t compute_address(CpuState *cpu_state, Instruction *instr, uint16_t offset);

#endif