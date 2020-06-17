#ifndef GUI_H_
#define GUI_H_

#include <ncurses.h>
#include <curses.h>
#include "str_list.h"

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

// Shows the memory map at the given address as a list of individual bytes
void update_memory_map_by_byte(MemoryWin *mem_win, uint8_t *memory, uint32_t address);

// Shows the memory map at a given address as a list of words (4 bytes)
void update_memory_map_by_word(MemoryWin *mem_win, uint8_t *memory, uint32_t address);


/*********** Output window functions ***********/

// Prints text to the output window (no need to refresh)
void print_to_output(OutputWin *out_win, char *text);


/*********** Input window functions ***********/

// Gets a single string from input (returned on enter press, ie: \n)
void get_user_input(InputWin *inp_win);

#endif
