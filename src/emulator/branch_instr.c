#include <stdint.h>
#include "utilities.h"
#define PC 15


void execute_branch_instr(Instruction* instr, CpuState *cpu_state){
    uint32_t code = instr->code;
	uint32_t cond = process_mask(code, 28, 31);
	
       	if (!check_CPSR_cond(cond, cpu_state)){
		return;
	}
	// processing offset	
	int32_t offset = (int32_t )process_mask(code, 0, 23);
	offset <<= 2;
	bool sign = process_mask(offset, 25, 25);
	//uint32_t *pc = cpu_state->registers
	uint32_t pc = cpu_state->registers[PC];
	if (!sign){
		// positive number, no need for more calc
		pc = pc + offset;
		// cpu_state_clear_pipeline();
		return;
	}	
	// negative number, must sign extend
	uint32_t mask = ((1 << 32) - 1) - ((1 << 26) - 1);
	offset |= mask;
	pc = ((int32_t) pc + offset);
	// cpu_state_clear_pipeline();

}

