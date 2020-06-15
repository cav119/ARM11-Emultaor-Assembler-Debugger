#include "gui.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define REGS_WIN_HEIGHT 10
#define MIDDLE_WINS_HEIGHT 14
#define INPUT_WIN_HEIGHT 3

static RegistersWin *init_regs_win(uint32_t *regs, int width) {
    RegistersWin *regs_win = malloc(sizeof(RegistersWin));
    if (!regs_win) {
        perror("Alloc failed in init_regs_win");
        exit(EXIT_FAILURE);
    }
    regs_win->win = newwin(REGS_WIN_HEIGHT, width - 2, 0, 1);
    box(regs_win->win, 0, 0);
    update_registers(regs_win, regs);

    return regs_win;
}

static MemoryWin *init_memory_win(uint8_t *memory, int width, int regs_win_height) {
    MemoryWin *memory_win = malloc(sizeof(MemoryWin));
    if (!memory_win) {
        perror("Alloc failed in init_memory_win");
        exit(EXIT_FAILURE);
    }
    memory_win->win = newwin(MIDDLE_WINS_HEIGHT, 2 * width / 3 - 1, regs_win_height + 1, 1);
    box(memory_win->win, 0, 0);

    for (int i = 0; i < MIDDLE_WINS_HEIGHT - 2; i++) {
        mvwprintw(memory_win->win, i + 1, 1, "M[0x%.8x] = 0x%.2x ", i, 0);
    }

    return memory_win;
}

static HelpWin *init_help_win(int width, int regs_win_height) {
    HelpWin *hw = malloc(sizeof(HelpWin));
    if (!hw) {
        perror("Alloc failed in init_help_win");
        exit(EXIT_FAILURE);
    }

    WINDOW *help_win = newwin(MIDDLE_WINS_HEIGHT, width / 3, regs_win_height + 1, 2 * width / 3);
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

    hw->win = help_win;

    return hw;
}

static OutputWin *init_out_win(int width, int height, int mem_win_height) {
    OutputWin *out_win = malloc(sizeof(OutputWin));
    if (!out_win) {
        perror("Alloc failed in init_out_win");
        exit(EXIT_FAILURE);
    }
    out_win->output_history = create_list();
    out_win->win = newwin(height - mem_win_height - 5, width - 2, mem_win_height + 2, 1);
    mvwprintw(out_win->win, 1, 1, "Output of commands goes here...");
    box(out_win->win, 0, 0);

    return out_win;
}

static InputWin *init_inp_win(int width, int out_win_height) {
    InputWin *inp_win = malloc(sizeof(InputWin));
    if (!inp_win) {
        perror("Alloc failed in init_inp_win");
        exit(EXIT_FAILURE);
    }
    inp_win->win = newwin(INPUT_WIN_HEIGHT, width - 2, out_win_height + 3, 1);
    box(inp_win->win, 0, 0);   // set border

    strcpy(inp_win->prompt_str, PROMPT_TXT);
    inp_win->input_str_len = inp_win->win->_maxx - 1;
    inp_win->input_str = calloc(inp_win->input_str_len, sizeof(char));

    mvwprintw(inp_win->win, 1, 1, "%s", inp_win->prompt_str);
    keypad(inp_win->win, true);    

    return inp_win;
}


MainWin *init_main_win(uint32_t *regs, uint8_t *memory) {
    initscr();
    cbreak();
    scrollok(stdscr, false);

    int width = getmaxx(stdscr);
    int height = getmaxy(stdscr);
    MainWin *win = malloc(sizeof(MainWin));
    if (!win) {
        perror("Alloc failed in init_main_win");
        exit(EXIT_FAILURE);
    }

    win->regs_win = init_regs_win(regs, width);
    win->memory_win = init_memory_win(memory, width, win->regs_win->win->_maxy);
    win->help_win = init_help_win(width, win->regs_win->win->_maxy);
    win->out_win = init_out_win(width, height, win->memory_win->win->_maxy + win->regs_win->win->_maxy);
    win->inp_win = init_inp_win(width, win->out_win->win->_maxy + win->memory_win->win->_maxy + win->regs_win->win->_maxy);

    return win;
}

void refresh_main_win(MainWin *win) {
    refresh();
    wrefresh(win->regs_win->win);
    wrefresh(win->memory_win->win);
    wrefresh(win->help_win->win);
    wrefresh(win->out_win->win);
    wrefresh(win->inp_win->win);
}

void destroy_main_win(MainWin *win) {
    free(win->regs_win);
    free(win->memory_win);
    free(win->help_win);
    list_destroy(win->out_win->output_history, free);
    free(win->out_win);

    free(win->inp_win->input_str);
    free(win->inp_win);

    free(win);
}


/************ REGISTER WINDOW FUNCTIONS ************/

void update_registers(RegistersWin *regs_win, uint32_t *registers) {
    int width = regs_win->win->_maxx;
    int height = regs_win->win->_maxy;

    for (int i = 0; i < 8; i++) {
        mvwprintw(regs_win->win, i + 1, 1, "r%d  = 0x%.8x", i, registers[i]);
    }
    for (int i = 8; i < 17; i++) {
        if (i == 13 || i == 14) {
            continue;
        }
        if (i == 15) {
            mvwprintw(regs_win->win, i + 1 - height, width / 2 - 1, 
                "PC   = 0x%.8x", registers[i]);
        } else if (i == 16) {
            mvwprintw(regs_win->win, i + 1 - height, width / 2 - 1, 
                "CPSR = 0x%.8x", registers[i]);
        } else {
            mvwprintw(regs_win->win, i + 2 - height, width / 2 - 1, 
                i < 10 ? "r%d   = 0x%.8x" : "r%d  = 0x%.8x", i, registers[i]);
        }
    }

}

/************ MEMORY WINDOW FUNCTIONS ************/

// maybe be draw little endian table like in a hex editor?
void update_memory_map(MemoryWin *mem_win, uint8_t *memory, uint32_t address) {
    int midpoint = (MIDDLE_WINS_HEIGHT - 2) / 2;
    // replace for total memory size

    // Check edge cases
    int start, stop;
    if (((int32_t) address) - midpoint + 1 < 0) {
        start = 0;
        stop = MIDDLE_WINS_HEIGHT - 2;
    } else if (((int32_t) address) + midpoint + 1 > 1000) {
        start = 1000 - (MIDDLE_WINS_HEIGHT - 2);
        stop = 1000;
    }
    else {
        start = address - midpoint + 1;
        stop = address + midpoint + 1;
    }

    for (int i = start; i < stop; i++) {
        if (address == i) {
            wattron(mem_win->win, A_REVERSE);
            mvwprintw(mem_win->win, i + 1 - start, 1, "-> M[0x%.8x] = 0x%.2x ", i, memory[i]);
            wattroff(mem_win->win, A_REVERSE);
        } else {
            mvwprintw(mem_win->win, i + 1 - start, 1, "   M[0x%.8x] = 0x%.2x ", i, memory[i]);
        }
    }
    wrefresh(mem_win->win);
}


/************ OUPUT WINDOW FUNCTIONS ************/

void print_to_output(OutputWin *out_win, char *text) {
    wclear(out_win->win);
    box(out_win->win, 0, 0);

    // Store and print the output of the last command
    char *text_cpy = calloc(strlen(text) + 1, sizeof(char));
    if (!text_cpy) {
        perror("Alloc failed in print_to_output");
        exit(EXIT_FAILURE);
    }
    strcpy(text_cpy, text);
    wattron(out_win->win, A_BOLD);
    mvwprintw(out_win->win, out_win->win->_maxy - 1, 1, "%s", text_cpy);
    wattroff(out_win->win, A_BOLD);
    list_append(out_win->output_history, text_cpy);

    // Print output history
    for (int i = 1; i < out_win->win->_maxy - 1; i++) {
        char *old = list_get_index(out_win->output_history, out_win->output_history->size - i - 1);
        if (old) {
            mvwprintw(out_win->win, out_win->win->_maxy - i - 1, 1, "%s", old);
        }
    }

    wrefresh(out_win->win);
}


/************ INPUT WINDOW FUNCTIONS ************/

void get_user_input(InputWin *inp_win) {
    wgetnstr(inp_win->win, inp_win->input_str, inp_win->input_str_len - PROMPT_SIZE - 1);
    wclear(inp_win->win);
    box(inp_win->win, 0, 0);
    mvwprintw(inp_win->win, 1, 1, "%s", inp_win->prompt_str);
    wrefresh(inp_win->win);
}


