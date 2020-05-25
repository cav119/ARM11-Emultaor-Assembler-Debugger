#include <stdint.h>
#include "utilities.h"

#define CONDITION_BITS process_mask(instr->code, 28, 31)
#define OFFSET_BITS process_mask(instr->code, 0, 23)

void execute_branch_instr(Instruction* instr, CpuState *cpu_state){
	
    if (!check_CPSR_cond(CONDITION_BITS, cpu_state)){
		return;
	}
	// processing offset	
	int32_t offset = (int32_t) OFFSET_BITS;
	offset <<= 2;
	bool sign = process_mask(offset, 25, 25);
	//uint32_t *pc = cpu_state->registers
	if (!sign){
		// positive number, no need for more calc
		// cpu_state_clear_pipeline();
		offset_pc(offset, cpu_state);
		return;
	}	
	// negative number, must sign extend
	uint32_t mask = ((1 << 32) - 1) - ((1 << 26) - 1);
	offset |= mask;
	offset_pc(offset, cpu_state);
	// cpu_state_clear_pipeline();
}

