#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>

#define PROMPT_TXT "> "
#define PROMPT_SIZE 2

#define RUN_CMD_L "run"
#define RUN_CMD_S "r"
#define NEXT_CMD_L "next"
#define NEXT_CMD_S "n"
#define BREAK_CMD_S "b"
#define BREAK_CMD_L "break"
#define PRINT_CMD_S "p"
#define PRINT_CMD_L "print"

/*

Add: run
Add: n
Add: help

-> help n run

*/




void exit_program_handler(int s) {
    // puts("Program execution quitted");
    // sleep(1);
    exit(EXIT_SUCCESS);
}

// Cleanup goes here, called whenever program exits
void destroy_gui(void) {
    endwin();
}

int main(int argc, char **argv) {
    // assert(argc > 1);

    // reassign Ctr+C such that it quits the prog exec, but not the debugger
    signal(SIGINT, &exit_program_handler);

    atexit(&destroy_gui);

    // Initialise ncurses GUI
    initscr();
    cbreak();

    // get the height and width of the screen
    int height = getmaxy(stdscr); 
    int width = getmaxx(stdscr);

    // window for displaying registers (top)
    WINDOW *regs_win = newwin(10, width - 2, 0, 1);
    box(regs_win, 0, 0);
    // 0-12 are gp regs
    for (int i = 0; i < 8; i++) {
        mvwprintw(regs_win, i + 1, 1, "r%d  = 0x%.8x", i, i + rand() % 20);
    }
    for (int i = 8; i < 17; i++) {
        if (i == 13 || i == 14) {
            continue;
        }
        if (i == 15) {
            mvwprintw(regs_win, i + 1 - regs_win->_maxy, regs_win->_maxx / 2 - 1, 
                "PC   = 0x%.8x", i + rand() % 20);
        } else if (i == 16) {
            mvwprintw(regs_win, i + 1 - regs_win->_maxy, regs_win->_maxx / 2 - 1, 
                "CPSR = 0x%.8x", i + rand() % 20);
        } else {
            mvwprintw(regs_win, i + 2 - regs_win->_maxy, regs_win->_maxx / 2 - 1, 
                i < 10 ? "r%d   = 0x%.8x" : "r%d  = 0x%.8x", i, i + rand() % 20);
        }
    }
    // pc is 15, cpsr is 16

    // window that shows the assembly code (defined here bc dimensions based on prev)
    WINDOW *memory_win = newwin(14, 2 * width / 3 - 1, regs_win->_maxy + 1, 1);
    scrollok(memory_win, TRUE);
    for (int i = 0; i < 100; i++) {
        mvwprintw(memory_win, 1 + i, 1, "%d - some random line", i);
    }
    box(memory_win, 0, 0);

    // window that shows the assembly code (defined here bc dimensions based on prev)
    WINDOW *help_win = newwin(14, width / 3, regs_win->_maxy + 1, 2 * width / 3);
    wattron(help_win, A_BOLD);
    mvwprintw(help_win, 1, help_win->_maxx / 2 - strlen("--- COMMANDS ---") / 2, "--- COMMANDS ---");
    wattroff(help_win, A_BOLD);
    mvwprintw(help_win, 3, 1, "Run program:      %s | %s", RUN_CMD_S, RUN_CMD_L);
    mvwprintw(help_win, 4, 1, "Next instruction: %s | %s", NEXT_CMD_S, NEXT_CMD_L);
    mvwprintw(help_win, 5, 1, "Set breakpoint:   %s | %s", BREAK_CMD_S, BREAK_CMD_L);
    mvwprintw(help_win, 6, 1, "Print memory:     %s | %s [-a]", PRINT_CMD_S, PRINT_CMD_L);
    wattron(help_win, A_ITALIC);
    mvwprintw(help_win, 7, 1, "    -a flag shows nearby memory", PRINT_CMD_S, PRINT_CMD_L);
    wattroff(help_win, A_ITALIC);
    mvwprintw(help_win, help_win->_maxy - 2, 1, "Scroll through command history\n using arrow keys");
    box(help_win, 0, 0);

    // // print some sample code
    // char *sample_code[] = {
    //     "mov r0,#1", "mov r1,#5", "loop:", "mul r2,r1,r0", "mov r0,r2", "sub r1,r1,#1",
    //     "cmp r1,#0", "bne loop", "mov r3,#0x100", "str r2,[r3]",
    //     NULL
    // };
    // int selected = 1;
    // for (int i = 0; sample_code[i]; i++) {
    //     if (i == selected) {
    //         wattron(code_win, A_REVERSE);
    //         mvwprintw(code_win, i + 1, 1, "-> %s", sample_code[i]);
    //         wattroff(code_win, A_REVERSE);
    //         continue;
    //     }
    //     mvwprintw(code_win, i + 1, 1, "   %s", sample_code[i]);
    // }

    // window for showing result information from command (on top of input box)
    WINDOW *output_win = newwin(14, width - 2, height - 17, 1);
    mvwprintw(output_win, 1, 1, "Output of commands goes here...");
    box(output_win, 0, 0);
    
    // window for the user input (bottom)
    // args are: rows (height), cols (width), start_y, start_x
    WINDOW *input_win = newwin(3, width - 2, height - 3, 1);
    box(input_win, 0, 0);   // set border
    char prompt[] = PROMPT_TXT;
    mvwprintw(input_win, 1, 1, "%s", prompt); // print prompt
    keypad(input_win, true);    // enable keypad

    char input_text[input_win->_maxx - 1]; // stores the user string


    refresh();
    wrefresh(regs_win);
    wrefresh(memory_win);
    wrefresh(help_win);
    wrefresh(output_win);
    wrefresh(input_win);

    // MAIN LOOP
    while (1) {
        // this is a blocking operation, so we may need different threads
        // to look for: user input, arrow keys for command history
        get_user_input(input_win, input_text, sizeof(input_text), prompt); 

        parse_command(input_text, output_win);

        // clear input window and redraw prompt, ready for new command
        wclear(input_win);
        box(input_win, 0, 0);
        mvwprintw(input_win, 1, 1, "%s", prompt);
        wrefresh(input_win);
    }


    return EXIT_SUCCESS;
}


// Gets a single string from input (returned on enter press, ie: \n)
void get_user_input(WINDOW *win, char *buffer, int buffer_size, char *prompt) {
    wgetnstr(win, buffer, buffer_size - PROMPT_SIZE - 1);
    // wclear(win);
    // box(win, 0, 0);
    // mvwprintw(win, 1, 1, "%s", prompt);
    // wrefresh(win);
}


// Parses commands and executes them where possible
void parse_command(char *command, WINDOW *out_win) {
    // do any extra formatting, ie: ignore spaces

    if (strcasecmp(command, RUN_CMD_S) == 0 || strcasecmp(command, RUN_CMD_L) == 0) {
        // do something...
        print_to_output(out_win, "Started program execution");
    } else if (strcasecmp(command, NEXT_CMD_S) == 0 || strcasecmp(command, NEXT_CMD_L) == 0) {
        print_to_output(out_win, "Moved to next instruction");
    }
}

void print_to_output(WINDOW *out_win, char *text) {
    wclear(out_win);
    box(out_win, 0, 0);
    mvwprintw(out_win, 1, 1, "%s", text);
    wrefresh(out_win);
}

// void get_user_input(WINDOW *win, char *buffer, int buffer_size, char *prompt) {
//     int c = getch();
//     int i = 0;

//     // need to handle backspaces

//     while (i < buffer_size - PROMPT_SIZE - 1 && c != '\n') {
//         buffer[i] = (char) c;
//         // draw to screen
//         mvwprintw(win, 1, i + PROMPT_SIZE + 1, "%c", (char) c);
//         wrefresh(win);
//         // get next char
//         c = getch();
//         i++;
//     }
//     buffer[i] = '\0';

//     wclear(win);
//     box(win, 0, 0);
//     mvwprintw(win, 1, 1, prompt); // clear the prompt on wgetstr
//     wrefresh(win);
// } 

