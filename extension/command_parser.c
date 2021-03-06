#include "command_parser.h"
#include "../src/emulator/pipeline_executor.h"

static char *print_bits_32(uint32_t bits) {
    char *bit_str = calloc(33, sizeof(char));
    uint32_t mask = 1 << 31;
    for (int i = 0; i < 32; i++){
        if (mask & bits) {
            bit_str[i] = '1';
        }
        else {
            bit_str[i] = '0';
        }
        bits <<= 1;
    }
    bit_str[32] = '\0';
    return bit_str;
}


static bool same_str(char *str1, char *str2){
    if (!str1 || !str2){
        return false;
    }
    return strcmp(str1, str2) == 0;
}


static char* clone_str(char *str){
    if (str == NULL){
        return NULL;
    }
    int len = strlen(str);
    char *clone = calloc(len + 1, sizeof(char));
    for (int i = 0; i < len; i++){
        clone[i] = str[i];
    }
    // no reason to add '\0' since memory was calloced
    return clone;
}

static void free_tokens(char **tokens, int length){
    for (int i = 0; i < length; i++) {
        free(tokens[i]);
    }
    free(tokens);
}

static char **tokenize_instr(char *instr, int args){
    char **tokens = calloc(args, sizeof(char) * MAX_COMMAND_LEN);
    int curr = 0;
    char *tok = strtok(instr, " ");
    while (tok){
        tokens[curr] = clone_str(tok);
        curr++;
        tok = strtok(NULL, " ");
    }
    if (curr != args){
        for (int i = 0; i < curr; i++){
            free(tokens[i]);
        }
        free(tokens);
        return NULL;
    }

    return tokens;
}

static bool only_numbers_str(char *str){
    if (!str){
        return NULL;
    }
    for (; *str; str++){
        if (! (*str >= '0' && *str <= '9') ){
            return false;
        }
    }
    
    return true;
}

static PrintCommand *tokens_to_print_comm(char **tokens){
    if (!tokens[0] || !tokens[1] || !tokens[2]){
        // one of the tokens is null
        return NULL;
    }
    if (!same_str(tokens[0], PRINT_CMD_S) && !same_str(tokens[0], PRINT_CMD_L)){
        // first token isn't 'print' or 'p'
        return NULL;
    }
    PrintCommand *comm = calloc(1, sizeof(PrintCommand));
    if (same_str(tokens[1], "HEX")){
        comm->format = HEX;
    }
    else if (same_str(tokens[1], "BIN")){
        comm->format = BIN;
    }
    else if (same_str(tokens[1], "DEC")){
        comm->format = DEC;
    }
    else {
        // doesn't match any format
        free(comm);
        return NULL;
    }
    char *last = tokens[2];
    int last_len = strlen(last);
    if (last_len < 2){
        free(comm);
        return NULL;
    }
    if (last[0] == 'R'){
        comm->is_register_print = true;
    }
    else if (last[0] == 'M'){
        comm->is_register_print = false;
    }
    else {
        // wrong format
        free(comm);
        return NULL;
    }
    if (!only_numbers_str(last+1)){
        // has non-number characters
        free(comm);
        return NULL;
    }
    comm->print_target = atoi(last + 1);
    return comm;

}

static ExecutableCommand *parse(char *input,  HashTable *ht){
    char *copy = calloc(sizeof(char),  MAX_COMMAND_LEN);
    strcpy(copy, input);
    ExecutableCommand *comm = calloc(1, sizeof(ExecutableCommand));
    // could be handled more nicely with a hashtable
    // but there's no reason to allocate so much memory just for 4 command types
    if (same_str(input, HELP_CMD_L)){
        comm->type = HELP_CMD;
    }
    else if (same_str(input, RUN_CMD_L) || same_str(input, RUN_CMD_S)){
        comm->type = RUN_CMD; 
    }
    else if (same_str(input, NEXT_CMD_L) || same_str(input, NEXT_CMD_S)){
        comm->type = NEXT_CMD;
    }
    else if (same_str(input, EXIT_CMD_S) || same_str(input, EXIT_CMD_L)){
        comm->type = EXIT_CMD;
    }
    else if (same_str(strtok(copy, " "), BREAK_CMD_S) || same_str(strtok(copy, " "), BREAK_CMD_L)) {
        comm->type = BREAK_CMD;
        char **tokens = tokenize_instr(input, 2);
        if(ht == NULL){
            //invalid list
            perror("null table");
            free(comm);
            free_tokens(tokens, 2);
            free(copy);
            return NULL;
        }
        if(tokens == NULL){
            //invalid break
            free(comm);
            free(copy);
            return NULL;
        }
        if (!only_numbers_str(tokens[1])){
            //not a number
            free(comm);
            free_tokens(tokens, 2);
            free(copy);
            return NULL;
        }
        uint32_t *break_cmd = malloc(sizeof(uint32_t));
        *break_cmd = atoi(tokens[1]);
        if (*break_cmd % 4 != 0){
            puts("The memory address must be a multiple of 4 due to the 32 bit system nature\n");
            free(comm);
            free(break_cmd);
            free_tokens(tokens, 2);
            free(copy);
            return NULL;
        }
        bool *is_active = malloc(sizeof(bool));
        *is_active = true;
        ht_set(ht, break_cmd, is_active, sizeof(uint32_t) / sizeof(char));
        free_tokens(tokens, 2);
    }
    else {
        // print command
        comm->type = PRINT_CMD;
        char **tokens = tokenize_instr(input, 3);
        if (tokens == NULL){
            // invalid instruction
            free(comm);
            free(copy);
            return NULL;
        }
        PrintCommand *print = tokens_to_print_comm(tokens);
        free_tokens(tokens, 3);

        if (!print){
            free(comm);
            free(copy);
            return NULL;
        }
        comm->command.print = print;
    }
    free(copy);
    return comm;
}

#define MAX_REG (16)
#define MAX_MEMORY (65532)

static void execute_print_comm(PrintCommand *comm, CpuState *cpu_state, MainWin *win){
    if (!comm){
        return;
    }
    uint32_t target;
    if (comm->is_register_print){
        if (!(comm->print_target <= MAX_REG)){
            print_to_output(win->out_win, "Invalid register number");
            wmove(win->inp_win->win, 1, 1 + PROMPT_SIZE);
            return;
        }
        target = cpu_state->registers[comm->print_target];
    }
    else {
        // print little endian memory
        if (!(comm->print_target <= MAX_MEMORY) || (comm->print_target % 4 != 0)){
            print_to_output(win->out_win, "Invalid memory address");
            wmove(win->inp_win->win, 1, 1 + PROMPT_SIZE);
            return;
        }
        target = fetch_big_endian(comm->print_target, cpu_state);
    }
   
    if (comm->format == BIN){
        char *bit_str = print_bits_32(target);
        print_to_output(win->out_win, bit_str);
        wrefresh(win->out_win->win);
        wmove(win->inp_win->win, 1, 1 + PROMPT_SIZE);
        free(bit_str);
    } else if (comm->format == HEX) {
        char *msg = malloc(sizeof(char) * win->out_win->win->_maxx - 3);
        sprintf(msg, "$: 0x%.8x", target);
        print_to_output(win->out_win, msg);
        wmove(win->inp_win->win, 1, 1 + PROMPT_SIZE);
        free(msg);
    } else {
        char *msg = malloc(sizeof(char) * win->out_win->win->_maxx - 3);
        sprintf(msg, "$: %d", target);
        print_to_output(win->out_win, msg);
        wmove(win->inp_win->win, 1, 1 + PROMPT_SIZE);
        free(msg);
    }
}

static bool execute_command(char *input, ExecutableCommand *comm, CpuState *cpu_state, MainWin *win){
    switch (comm->type)  {
        case HELP_CMD:
            print_to_output(win->out_win, "See all possible commands above");
            break;            
        case RUN_CMD:
            print_to_output(win->out_win, "Running the program...");
            break;
        case NEXT_CMD:
            print_to_output(win->out_win, "Stepping to the next instruction");
            break;
        case EXIT_CMD:
            print_to_output(win->out_win, "Exiting the program");
            return true;
        case BREAK_CMD:
            print_to_output(win->out_win, "Added breakpoint");
            break;
        case PRINT_CMD:
            execute_print_comm(comm->command.print, cpu_state, win); 
            break; 
    }
    wmove(win->inp_win->win, 1, 1 + PROMPT_SIZE);
    return false;
}


bool get_input_and_execute(CpuState *cpu_state, bool *is_stepping, HashTable *ht, MainWin *win) {

    get_user_input(win->inp_win); // get the user input from the gui

    ExecutableCommand *comm = parse(win->inp_win->input_str, ht);
    if (comm == NULL){
        print_to_output(win->out_win, "Invalid command, see the available commands above.");
        wmove(win->inp_win->win, 1, 1 + PROMPT_SIZE);
        return get_input_and_execute(cpu_state, is_stepping, ht, win);
    }
    bool ending = execute_command(win->inp_win->input_str, comm, cpu_state, win);
    if (!ending && comm->type != NEXT_CMD && comm->type != RUN_CMD) {
        if (comm->command.print){
            free(comm->command.print);
        }
        free(comm);
        return get_input_and_execute(cpu_state, is_stepping, ht, win);
    }
    if (comm->type == RUN_CMD) {
        *is_stepping = false;
    }
    if (ending) {
        free(comm);
        return true;
    }
    free(comm);
    return false;
}
