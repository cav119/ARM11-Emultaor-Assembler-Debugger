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
	// bits 27-24 (7-4 on l. endian) are 1010 (0101 little endian) 
	return process_mask(bits, 4, 7) == 5;	
}	

bool is_multiply_instr(uint32_t bits){
	// bits 27-22 (4-9 little endian) are all 0
	bool top_bits_zero = process_mask(bits, 4, 9) == 0;
	// bits 7-4 (27-24  little endian) are 1001 (1001 little endian) 
	bool lower_bits = process_mask(bits, 24, 27) == 9;
	return top_bits_zero && lower_bits;
}

bool is_single_data_transfer_instr(uint32_t bits){
	// bits 27-26 (4-5 little endian) are 01 (10 little endian)
	return process_mask(bits, 4, 5) == 2; 
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

