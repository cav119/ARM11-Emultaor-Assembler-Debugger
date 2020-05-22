#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define NUM_REGISTERS 17
#define MEMORY_SIZE 65536
#define PC 15
#define CPSR 16
#define MAX_INDEX 31

// CpuState struct
typedef struct {
	uint32_t *registers;
	uint8_t *memory;
} CpuState;

// Might not need this
typedef enum {
	data_process,
	multiply,
	single_data_transfer,
	branch
} instruction_type;

typedef struct {
	uint32_t code;
	instruction_type type;	
} instruction;

typedef enum {N, Z, C, V} flag;


// make utilities.c and add this
// Utility func: checks if pointer is null and exits with error message if so
void check_ptr_not_null(void *ptr, char error_msg[]) {
	if (!ptr) {
		printf("%s\n", error_msg);
		exit(EXIT_FAILURE);
	}
}

// Create a new CPU and an initialise the registers/memory to zero
CpuState *cpu_state_init(void) {
	CpuState *cpu_state = (CpuState *) malloc(sizeof(CpuState));
	check_ptr_not_null(cpu_state, "Could not allocate cpu state memory");

	// allocate and init registers to 0
	uint8_t *allocated_regs = calloc(NUM_REGISTERS, sizeof(uint32_t));
	check_ptr_not_null(allocated_regs, "Could not allocate and initialise registers");
	cpu_state->registers = (uint32_t *) allocated_regs;

	// allocate and init memory to 0
	uint8_t *allocated_memory = calloc(MEMORY_SIZE, sizeof(uint8_t));
	check_ptr_not_null(allocated_memory, "Could not allocate and initialise RAM memory");
	cpu_state->memory = (uint8_t *) allocated_memory;

	return cpu_state;
}

// Frees the allocated memory in the CpuState
void cpu_state_free(CpuState *cpu_state){
	free(cpu_state->memory);
	free(cpu_state->registers);
	free(cpu_state);
}

// Prints the current state of all registers for debugging
void print_registers(CpuState *cpu_state){
    for (int i = 0; i < NUM_REGISTERS - 4; i++){
        printf("reg[%d] = 0x%.8x\n", i, cpu_state->registers[i]);
    }
	printf("PC = 0x%.8x\nCPSR = 0x%.8x\n", 
		cpu_state->registers[PC], 
		cpu_state->registers[CPSR]
	);
}

//sets given flag in CPSR to (0 /1);
void set_flag(CpuState *cpuState, flag flag, bool set){
    if (flag > 3){
        printf("wrong flag");
        exit(EXIT_FAILURE);
    }
    int mask = (1 << (MAX_INDEX - flag));
    cpuState->registers[CPSR] = set ?
            cpuState->registers[CPSR] | mask : cpuState -> registers[CPSR] & ~(mask);
}

//gets the value of the given flag
bool get_flag(CpuState *cpuState, flag flag){
    int mask = (1 << (MAX_INDEX - flag));
    return cpuState->registers[CPSR] & mask;
}


// Prints the current state of memory for debugging
void print_memory(CpuState *cpu_state, int from, int to){
    for (int i = from; i < to; i++){
        printf("Location 0x%.8x = 0x%.8x\n", i, cpu_state->memory[i]);
    }
}

// prints first instruction (little endian)
void print_first_instruction(CpuState *cpu_state) {
	uint8_t byte1 = cpu_state->memory[3];
	uint8_t byte2 = cpu_state->memory[2];
	uint8_t byte3 = cpu_state->memory[1];
	uint8_t byte4 = cpu_state->memory[0];

	printf("First instruction is: %.2x %.2x %.2x %.2x\n", byte1, byte2, byte3, byte4);
}

int main(int argc, char *argv[]) {
	assert(argc == 2);
	printf("%s\n", argv[1]);

	CpuState *cpu_state = cpu_state_init();

	// Load binary file data into CPU Memory
	FILE *prog_file = fopen(argv[1], "rb");                // Open the file in binary mode
	check_ptr_not_null(prog_file, "Could not open file.");

	fseek(prog_file, 0, SEEK_END);                         // Jump to the end of the file
	uint64_t file_length = ftell(prog_file);               // Get the current byte offset in the file
	rewind(prog_file);                                     // Jump back to the beginning of the file

	assert(file_length <= MEMORY_SIZE);                    // Program data has to fit into memory
	fread(cpu_state->memory, file_length, 1, prog_file);   // Read in the entire file
	fclose(prog_file); // Close the file


    print_registers(cpu_state);
	print_memory(cpu_state, 0, 16);
	print_first_instruction(cpu_state);

	cpu_state_free(cpu_state);
	return EXIT_SUCCESS;
}

