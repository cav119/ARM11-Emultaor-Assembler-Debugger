#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "emulator/cpu_state.h"
#include "emulator/utilities.h"
#include "emulator/pipeline_executor.h"
#include "emulator/pipeline_data.h"


int main(int argc, char *argv[]) {
    assert(argc == 2);

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
    size_t n = fread(cpu_state->memory, file_length, 1, prog_file); 
    assert(n != 0);
    fclose(prog_file);
    

    // Start execution of program
    // The 2nd and 3rd params indicate that the extension is not running
    start_pipeline(cpu_state, false, NULL);

    // Print state of program at the end
    print_registers(cpu_state);
    print_nonzero_big_endian_memory(cpu_state, MEMORY_SIZE);

    cpu_state_free(cpu_state);

    return EXIT_SUCCESS;
}
