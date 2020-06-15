#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#include "gui.h"


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
        // memory[i] = rand() % 0xFF;
        memory[i] = i;
    }

    // Pass in the CPuState struct instead
    // Create and render the main window w/ all its subwindows
    MainWin *main_win = init_main_win(registers, memory);
    refresh_main_win(main_win);


    // MAIN EVENT LOOP
    while (1) {

        get_user_input(main_win->inp_win); 
        parse_command(main_win->inp_win->input_str, main_win, registers, memory);

        // move the cursor back to the input window
        wmove(main_win->inp_win->win, 1, 1 + PROMPT_SIZE);
    }

    destroy_main_win(main_win);

    return EXIT_SUCCESS;
}

void navigate_cmd_history(InputWin *inp_win, bool up) {
    wclear(inp_win->win);
    box(inp_win->win, 0, 0);
    char *command;
    if (up) {
        command = "this";
    } else {
        command = "that";
    }
    mvwprintw(inp_win->win, 1, 1, "%s%s", inp_win->prompt_str, command);
    strcpy(inp_win->input_str, command);
    wrefresh(inp_win->win);
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
