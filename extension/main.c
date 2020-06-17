#include <stdio.h>
#include <ncurses.h>
#include <curses.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#include "gui.h"
#include "command_parser.h"

#include "../src/emulator/cpu_state.h"
#include "../src/emulator/utilities.h"
#include "../src/emulator/pipeline_executor.h"


// Handle Ctrl+C smoothly
void exit_program_handler(int sig) {
    exit(EXIT_SUCCESS); // calls destroy_gui()
}

// Cleanup goes here, called whenever program exits
void destroy_gui(void) {
    endwin();
}

int main(int argc, char **argv) {
    // assert(argc > 1);

    // Make sure program terminates smoothly and clears the ncurses screen
    signal(SIGINT, &exit_program_handler);
    atexit(&destroy_gui);

    // Init some fake register and memory data (would be accessed from emulator code)
    uint32_t *registers = calloc(17, sizeof(uint32_t));
    for (int i = 0; i < 17; i++) {
        registers[i] = rand() % 0xFFFFFFFF;
    }
    uint8_t *memory = calloc(1000, sizeof(uint8_t));
    for (int i = 0; i < 1000; i++) {
        memory[i] = rand() % 0xFF;
    }

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


    // Print state of program for testing
    // print_registers(cpu_state);
    // print_nonzero_big_endian_memory(cpu_state, MEMORY_SIZE);


    // Pass in the CPuState struct instead
    // Create and render the main window w/ all its subwindows
    MainWin *main_win = init_main_win(registers, memory);
    refresh_main_win(main_win);

    // Start execution of program
    start_pipeline(cpu_state, true, main_win);

    // MAIN EVENT LOOP
    // while (1) {
    //     start_pipeline(cpu_state, true, main_win);
    //     // get_user_input(main_win->inp_win); 
    //     // parse_command(main_win->inp_win->input_str, main_win, registers, memory);

    //     // move the cursor back to the input window
    //     // wmove(main_win->inp_win->win, 1, 1 + PROMPT_SIZE);
    // }

    destroy_main_win(main_win);
    cpu_state_free(cpu_state);

    return EXIT_SUCCESS;
}


// Parses commands and executes them where possible
void parse_command(char *command, MainWin *main_win, uint32_t *registers, uint8_t *memory) {
    // do any extra formatting, ie: ignore spaces
    srand(time(NULL));

    char *first = strtok(command, " ");
    char *snd = strtok(NULL, " ");

    // Check single word commands
    if (strcasecmp(command, RUN_CMD_S) == 0 || strcasecmp(command, RUN_CMD_L) == 0) {
        // do something...
        print_to_output(main_win->out_win, "Started program execution");

    } else if (strcasecmp(command, NEXT_CMD_S) == 0 || strcasecmp(command, NEXT_CMD_L) == 0) {
        print_to_output(main_win->out_win, "Moved to next instruction");

    } else if (strcasecmp(command, "reg") == 0) {
        for (int i = 0; i < 17; i++) {
            registers[i] = rand() % 0xFFFFFFFF;
        }
        registers[0] = 0xFFFFFFFF;
        update_registers(main_win->regs_win, registers);
        wrefresh(main_win->regs_win->win);

    } else if (strcasecmp(first, "mem") == 0) {
        if (!snd) {
            print_to_output(main_win->out_win, "Please specify a memory address");
        }
        uint32_t address = atoi(snd);
        update_memory_map_by_word(main_win->memory_win, memory, address);

    } else if (strcmp(command, "") == 0) {
        return;

    } else {
        print_to_output(main_win->out_win, "Invalid command, see help above.");
    }
}
