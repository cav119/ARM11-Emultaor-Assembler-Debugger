#include <stdint.h>
#include <stdlib.h>
#include "utilities.h"
#include "cpu_state.h"
#include "pipeline.h"
#include <stdbool.h>
void start_pipeline(CpuState *cpu_state) {
    // TODO: fetch decode execute loop
}

bool is_branch_instr(uint32_t bits){
	// bits 27-24  are 1010
	return process_mask(bits, 24, 27) == 10;
}	

bool is_multiply_instr(uint32_t bits){
	// bits 27-22  are all 0
	bool top_bits_zero = process_mask(bits, 22, 27) == 0;
	// bits 7-4  are 1001
	bool lower_bits = process_mask(bits, 4, 7) == 9;
	return top_bits_zero && lower_bits;
}

bool is_single_data_transfer_instr(uint32_t bits){
	// bits 27-26 ) are 01
	return process_mask(bits, 26, 27) == 1;
}

Instruction *decode_instruction(uint32_t bits){ 
	Instruction *instr = malloc(sizeof(Instruction));
	if (is_branch_instr(bits)){
		instr->type = branch;
	}
	else if (is_multiply_instr(bits)){
		instr->type = multiply;
	}
	else if (is_single_data_transfer_instr(bits)){
		instr->type = single_data_transfer; 
	}
	else {
		instr->type = data_process;
	}
	return instr;
}

