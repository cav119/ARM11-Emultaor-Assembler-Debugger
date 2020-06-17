#ifndef GUI_H_
#define GUI_H_

#include <ncurses.h>

#include "str_list.h"
#include "commands.h"

#define MEMORY_SIZE 65536
#define NUM_REGISTERS 17
#define PC 15
#define CPSR 16
#define PC_LAG 8

#define REGS_WIN_HEIGHT 10
#define MIDDLE_WINS_HEIGHT 14
#define INPUT_WIN_HEIGHT 3

typedef struct {
    WINDOW *win;
} RegistersWin;

typedef struct {
    WINDOW *win;
} MemoryWin;

typedef struct {
    WINDOW *win;
} HelpWin;

typedef struct {
    WINDOW *win;
    List *output_history;
} OutputWin;

typedef struct {
    WINDOW *win;
    char prompt_str[PROMPT_SIZE + 1];
    char *input_str;
    int input_str_len;
} InputWin;

typedef struct {
    RegistersWin *regs_win;
    MemoryWin *memory_win;
    HelpWin *help_win;
    OutputWin *out_win;
    InputWin *inp_win;
} MainWin;


// Initialises the GUI main window
MainWin *init_main_win(uint32_t *regs, uint8_t *memory);

// Renders the main window (refresh stdscr and all subwindows)
void refresh_main_win(MainWin *win);

// Frees resources used by MainWin struct
void destroy_main_win(MainWin *win);

/*********** Register window functions ***********/

// Updates the register window with the new contents of the registers
// Note: you must manually refresh the window after this call
void update_registers(RegistersWin *regs_win, uint32_t *registers);

/*********** Memory window functions ***********/

// Shows the memory map at a given address as a list of words (4 bytes)
void update_memory_map_by_word(MemoryWin *mem_win, uint8_t *memory, uint32_t address);


/*********** Output window functions ***********/

// Prints text to the output window (no need to refresh)
void print_to_output(OutputWin *out_win, char *text);


/*********** Input window functions ***********/

// Gets a single string from input (returned on enter press, ie: \n)
void get_user_input(InputWin *inp_win);

#endif
