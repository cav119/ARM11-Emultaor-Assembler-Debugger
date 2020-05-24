#ifndef DATA_PROC_INSTR_H
#define DATA_PROC_INSTR_H
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "cpu_state.h"
#include "pipeline.h"
#include "utilities.h"

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

typedef enum {
    lsl,
    lsr,
    asr,
    ror
} shift_op;


void execute_data_processing_instr(CpuState *cpu_state, Instruction *instr);

//Performs arithmetic shift right on operand by shift_amount times and returns the result
uint32_t arithmetic_shift_right(uint32_t operand, uint32_t shift_amount); 

//Performs rotate right on operand by rotate_amount times and returns the result
uint32_t rotate_right(uint32_t operand, uint32_t rotate_amount);

#endif
