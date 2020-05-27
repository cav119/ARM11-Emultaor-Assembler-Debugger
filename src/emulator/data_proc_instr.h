#ifndef DATA_PROC_INSTR_H
#define DATA_PROC_INSTR_H
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include "cpu_state.h"
#include "pipeline_data.h"
#include "utilities.h"
#include "barrel_shifter.h"

// Opcode enum
typedef enum {
    and = 0,
    eor = 1,
    sub = 2,
    rsb = 3,
    add = 4,
    tst = 8,
    teq = 9,
    cmp = 10,
    orr = 12,
    mov = 13
} opcode_type;


// Executes a data processing type instruction
void execute_data_processing_instr(CpuState *cpu_state, Instruction *instr);

#endif
