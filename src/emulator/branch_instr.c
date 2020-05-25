#include <stdint.h>
#include "utilities.h"
#include <stdlib.h>
#include "pipeline_executor.h"
#define CONDITION_BITS process_mask(instr->code, 28, 31)
#define OFFSET_BITS process_mask(instr->code, 0, 23)


bool execute_branch_instr(Instruction* instr, CpuState *cpu_state, Pipe* pipe) {

    if (!check_CPSR_cond(CONDITION_BITS, cpu_state)) {
        free(instr);
        pipe->executing = 0x0;
        return false;
    }
    // clearing pipeling because will jump

    // processing offset
    int32_t offset = (int32_t) OFFSET_BITS;
    offset <<= 2;
    bool sign = process_mask(offset, 25, 25);
    //uint32_t *pc = cpu_state->registers
    if (sign){
        // must be a negative number
        // sign extending number
        uint32_t mask = ((1 << 32) - 1) - ((1 << 26) - 1);
        offset |= mask;
    }
    offset_pc(offset, cpu_state);
    // negative number, must sign extend
    clear_pipe(pipe);
    pipe->fetching = fetch(cpu_state->registers[PC], cpu_state);
    return true;
}
