#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "utilities.h"
#include "pipeline_executor.h"
#include "pipeline_data.h"


Pipe *init_pipeline(CpuState *cpu_state) {
    Pipe *pipe = calloc(1, sizeof(Pipe));
    pipe->fetching = fetch(0, cpu_state);
    increment_pc(cpu_state);

    return pipe;
}


uint32_t fetch(uint32_t ptr, CpuState *cpu_state) {
    uint32_t code = 0;
    bool valid = check_valid_memory_access(cpu_state, ptr);
    if (!valid) {
        return 0;
    }

    for (int i = ptr; i < ptr + 4; i++) {
        code += (cpu_state->memory[i] << (8 * i));
    }
    return code;
}


bool is_branch_instr(uint32_t bits) {
	// bits 27-24  are 1010
	return process_mask(bits, 24, 27) == 10;
}	


bool is_multiply_instr(uint32_t bits) {
	// bits 27-22  are all 0
	bool top_bits_zero = process_mask(bits, 22, 27) == 0;
	// bits 7-4  are 1001
	bool lower_bits = process_mask(bits, 4, 7) == 9;
	return top_bits_zero && lower_bits;
}


bool is_single_data_transfer_instr(uint32_t bits) {
	// bits 27-26 are 01
	return process_mask(bits, 26, 27) == 1;
}


Instruction *decode_instruction(uint32_t bits) {
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
	instr->code = bits;
	return instr;
}


bool execute(Instruction *instruction, CpuState *cpu_state, Pipe *pipe) {
    if (!(check_CPSR_cond(process_mask(instruction->code, 28, 31), cpu_state))) {
        return false;
    }
    switch (instruction->type) {
        case data_process:
            execute_data_processing_instr(cpu_state, instruction);
            free(instruction);
            pipe->executing = 0x0;
            break;
        case multiply:
            execute_multiply_instruction(instruction, cpu_state);
            pipe->executing = 0x0;
            free(instruction);
            break;
        case single_data_transfer:
            execute_single_data_transfer_instr(cpu_state, instruction);
            free(instruction);
            pipe->executing = 0x0;
            break;
        case branch:
            // no need to free instruction because branch handles it
            return execute_branch_instr(instruction, cpu_state, pipe);
    }

    return true;
}


void start_pipeline(CpuState *cpu_state) {
    Pipe *pipe = init_pipeline(cpu_state);
    continue_pipe:
    while (pipe->fetching) {
        pipe->executing = pipe->decoding;
        pipe->decoding = decode_instruction(pipe->fetching);

        bool branch_instr_succeeded = false;
        if (pipe->executing) {
            instruction_type type = pipe->executing->type;
            bool succeeded = execute(pipe->executing,cpu_state, pipe);
            if (succeeded && type == branch) {
                branch_instr_succeeded = true;
            }
        }
        if (!branch_instr_succeeded) {
            pipe->fetching = fetch(cpu_state->registers[PC], cpu_state);
            increment_pc(cpu_state);
        }
    }

    // TODO: REMOVE GOTO, USE SOME OTHER TECHNIQUE TO LOOP BACK
    bool ended = end_pipeline(pipe, cpu_state);
    if (!ended) {
        goto continue_pipe;
    }
}


bool end_pipeline(Pipe *pipe, CpuState *cpu_state) {
    // Must have fetched a halt
    // since it stops when fetching a halt the block of code simulates executing 2 cycles
    // first executing pipe->executing, and then pipe->decoding
    if (pipe->executing != NULL) {
        // fetched a HALTH, must execute executing and then decoding
        instruction_type type = pipe->executing->type;
        bool succeeded = execute(pipe->executing, cpu_state, pipe);
        if (type == branch && succeeded) {
            return false;
        }
        increment_pc(cpu_state);
    } else {
        if (pipe->decoding != NULL) {
            instruction_type type = pipe->decoding->type;
            bool succeeded = execute(pipe->decoding, cpu_state, pipe);
            if (type == branch && succeeded) {
                return false;
            }
        }
    }

    increment_pc(cpu_state);
    free(pipe);

    return true;
}


void clear_pipe(Pipe *pipe){
    // clears the whole pipeline after a branch inst
    if (pipe->executing != NULL){
        free(pipe->executing);
    }
    if (pipe->decoding != NULL){
        free(pipe->decoding);
    }
    pipe->executing = 0x0;
    pipe->decoding = 0x0;
    pipe->fetching = 0x0;
}


void print_pipeline(Pipe *pipe){
    printf("%.8x\n", pipe->executing ? pipe->executing->code : 0);
    printf("%.8x\n", pipe->decoding ? pipe->decoding->code : 0);
    printf("%.8x\n", pipe->fetching);
}
