#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "emulator/cpu_state.h"
#include "emulator/utilities.h"
#include "emulator/pipeline.h"


// prints first instruction (little endian) as a test
void print_first_instruction(CpuState *cpu_state) {
	uint8_t byte1 = cpu_state->memory[3];
	uint8_t byte2 = cpu_state->memory[2];
	uint8_t byte3 = cpu_state->memory[1];
	uint8_t byte4 = cpu_state->memory[0];

	printf("First truction is: %.2x %.2x %.2x %.2x\n", byte1, byte2, byte3, byte4);
}

int main(int argc, char *argv[]) {
    setbuf(stdout, 0);
    uint32_t instr = 0xe3a02002;
    print_bits_32(instr);
    Instruction *ip = decode_instruction(instr);
    printf("%s\n", instr_to_string(ip->type));

	assert(argc == 2);
	printf("%s\n", argv[1]);

	// Open binary file and check if it was a valid path to file
	FILE *prog_file = fopen(argv[1], "rb");               
	check_ptr_not_null(prog_file, "Could not open file.");

	// Initialise the CpuState
	CpuState *cpu_state = cpu_state_init();

	// Get file length by moving cursor to the end and return back to start
	fseek(prog_file, 0, SEEK_END);                         
	uint64_t file_length = ftell(prog_file);               
	rewind(prog_file);                                     

	// Load program data into CPU memory, assuming it fits and close file
	assert(file_length <= MEMORY_SIZE);                    
	fread(cpu_state->memory, file_length, 1, prog_file); 
	fclose(prog_file); 

	// Start execution of program
	//start_pipeline(cpu_state);

	// Print state of program for testing
    print_registers(cpu_state);
	print_memory(cpu_state, 0, 16);
	print_first_instruction(cpu_state);

	cpu_state_free(cpu_state);

	return EXIT_SUCCESS;
}

