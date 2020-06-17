#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <stdbool.h>

#include "gui.h"
#include "command_parser.h"

#include "../src/emulator/cpu_state.h"
#include "../src/emulator/utilities.h"
#include "../src/emulator/pipeline_executor.h"


// Handle Ctrl+C signal smoothly
void exit_program_handler(int sig) {
    exit(EXIT_SUCCESS);
}

// Ncurses graceful cleanup (to avoid messing up the terminal)
void destroy_gui(void) {
    endwin();
}

int main(int argc, char **argv) {

    // Make sure program terminates smoothly and clears the ncurses screen
    signal(SIGINT, &exit_program_handler);
    atexit(&destroy_gui);

    assert(argc == 2);

    // Open binary file and check if it was a valid path to file
    FILE *prog_file = fopen(argv[1], "rb");               
    check_ptr_not_null(prog_file, "Could not open file.");

    // Initialise the emulator CpuState
    CpuState *cpu_state = cpu_state_init();

    // Get file length by moving cursor to the end and return back to start
    fseek(prog_file, 0, SEEK_END);                         
    uint64_t file_length = ftell(prog_file);               
    rewind(prog_file);                                     

    // Load program data into CPU memory assuming it fits and close file
    assert(file_length <= MEMORY_SIZE);                    
    size_t n = fread(cpu_state->memory, file_length, 1, prog_file); 
    assert(n != 0);
    fclose(prog_file);

    // Create and render the main window w/ all its subwindows using the CpuState
    MainWin *main_win = init_main_win(cpu_state->registers, cpu_state->memory);
    refresh_main_win(main_win);

    // Start execution of program
    start_pipeline(cpu_state, true, main_win);
    update_memory_map_by_word(main_win->memory_win, cpu_state->memory, cpu_state->registers[PC] - PC_LAG);
    update_registers(main_win->regs_win, cpu_state->registers);
    wrefresh(main_win->regs_win->win);

    print_to_output(main_win->out_win, "Finished running the program. Press any key to quit the debugger...");
    wmove(main_win->inp_win->win, 1, 1 + PROMPT_SIZE);
    wgetch(main_win->inp_win->win);    // wait for a key press

    cpu_state_free(cpu_state);
    destroy_main_win(main_win);

    return EXIT_SUCCESS;
}

