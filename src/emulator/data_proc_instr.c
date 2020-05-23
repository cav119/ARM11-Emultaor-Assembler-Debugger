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

void execute_data_processing_instr(CpuState *cpu_state, Instruction *instr) {
	//Assertions/checks
	assert(instr->type = data_process);

	//Variable definitions
	uint8_t immediate_enable_bit = process_mask(instr->code, 25, 25);
	uint8_t opcode = process_mask(instr->code, 21, 24);
	uint8_t cpsr_enable_bit = process_mask(instr->code, 20, 20);
	uint32_t operand_1 = *(cpu_state->registers + process_mask(instr->code, 16, 19));
	uint8_t dest_register = process_mask(instr->code, 12, 15);
	uint32_t operand_2 = process_mask(instr->code, 0, 11);
	//result will be the computed result that is written into the dest_register
	uint32_t result = 0;
	//if write_result is 0, then the result is NOT written to the dest_register
	uint8_t write_result = 1;
	//c_bit is 1 if 1 is to be written to the C bit of CPSR
	uint8_t c_bit = 0;

	//Compute Operand2
	if (immediate_enable_bit) {
		operand_2 = process_mask(instr->code, 0, 7);
		operand_2 = rotate_right(operand_2, process_mask(instr->code, 8, 11) * 2);
	} else {
		uint32_t shift_amount = process_mask(instr->code, 7, 11);
		//get the contents of register specified by bits 0-3
		uint32_t reg_contents = *(cpu_state->registers + process_mask(instr->code, 0, 3));

		//If bit 4 is 1:
		if (process_mask(instr->code, 4, 4)) {
			switch(process_mask(instr->code, 6, 5)) {
				case 0:
					operand_2 = logical_left(reg_contents, shift_amount);
					break;
				case 1:
					operand_2 = logical_right(reg_contents, shift_amount);
					break;
				case 2: 
					operand_2 = arithmetic_right(reg_contents, shift_amount);
					break;
				case 3:
					operand_2 = rotate_right(reg_contents, shift_amount);
					break;
				default:
					//default should not be reached - error
					exit(EXIT_FAILURE);
			}
		} else {
			//lower_byte of the register corresponding to bits 8-11
			uint8_t lower_byte = *(cpu_state->registers + process_mask(instr->code, 8, 11));
			switch(process_mask(instr->code, 6, 5)) {
				case 0:
					operand_2 = logical_left(reg_contents, lower_byte);
					break;
				case 1:
					operand_2 = logical_right(reg_contents, lower_byte);
					break;
				case 2: 
					operand_2 = arithmetic_right(reg_contents, lower_byte);
					break;
				case 3:
					operand_2 = rotate_right(reg_contents, lower_byte);
					break;
				default:
					//default should not be reached - error
					exit(EXIT_FAILURE);
			}
		}
		//CPSR C Flag
		if (cpsr_enable_bit) {
			//idk how to do this - check the carry out of the shift operation?
		}
	}

	//Execute
	switch (opcode) {
		case and:
			result = operand_1 & operand_2;	
			break;
		case eor:
			result = operand_1 ^ operand_2;
			break;
		case sub:
			result = operand_1 - operand_2;
			//set c_bit reminder
			break;
		case rsb:
			result = operand_2 - operand_1;
			//set c_bit reminder
			break;
		case add:
			result = operand_1 + operand_2;
			//set c_bit reminder
			break;
		case tst:
			result = operand_1 & operand_2;
			write_result = 0;
			break;
		case teq:
			result = operand_1 ^ operand_2;
			write_result = 0;
			break;
		case cmp:
			result = operand_1 - operand_2;
			write_result = 0;
			//set c_bit reminder
			break;
		case orr:
			result = operand_1 | operand_2;
			break;
		case mov:
			result = operand_2;
			break
		default:
			//Opcode should not enter this default area - means error
			exit(EXIT_FAILURE);
	}

	//Write to dest_register
	if (write_result) {
		*(cpu_state->registers + dest_register) = result;
	}

	//CPSR FLAGS
	if (cpsr_enable_bit) {
		//V bit unaffected
		
		//C bit (bit 29 of CPSR) - set to c_bit which is determined by the opcode:
		*(cpu_state->registers + CPSR) = *(cpu_state->registers + CPSR) | ((c_bit & 1) << 29);

		//Z bit (bit 30 of CPSR):
		//Z is 1 iff result == 0
		if (result == 0) {
			*(cpu_state->registers + CPSR) = *(cpu_state->registers + CPSR) | (1 << 30);
		} else {
			*(cpu_state->registers + CPSR) = *(cpu_state->registers + CPSR) ^ (*(cpu_state->registers + CPSR) & (1 << 30));
		}

		//N bit (bit 31 of CPSR) - set to bit 31 of result:
		*(cpu_state->registers + CPSR) = *(cpu_state->registers + CPSR) | ((1 << 31) & result);

	}
}

uint32_t rotate_right(uint32_t operand, uint32_t rotate_amount) {
	
}

uint32_t logical_left(uint32_t operand, uint32_t shift_amount) {

}

uint32_t logical_right(uint32_t operand, uint32_t shift_amount) {

}

uint32_t arithmetic_right(uint32_t operand, uint32_t shift_amount) {

}
