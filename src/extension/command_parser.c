

#include "command_parser.h"
#include "../emulator/pipeline_executor.h" 
#include "extension_utilities.h"

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

static char** tokenize_print_instr(char *instr){
    char **tokens = calloc(3, sizeof(char) * MAX_COMMAND_LEN);
    int curr = 0;
    char *tok = strtok(instr, " ");
    while (tok){
        tokens[curr] = clone_str(tok);
        curr++;
        tok = strtok(NULL, " ");
    }
    if (curr != 3){
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

static ExecutableCommand *parse(char *input){
    ExecutableCommand *comm = calloc(1, sizeof(ExecutableCommand));
    // could be handled more nicely with a hashtable
    // but there's no reason to allocate so much memory just for 4 command types
    if (same_str(input, RUN_CMD_L) || same_str(input, RUN_CMD_S)){
        comm->type = RUN_CMD; 
    }
    else if (same_str(input, NEXT_CMD_L) || same_str(input, NEXT_CMD_S)){
        comm->type = NEXT_CMD;
    }
    else if (same_str(input, BREAK_CMD_S) || same_str(input, BREAK_CMD_L)) {
        comm->type = BREAK_CMD;
    }
    else if (same_str(input, EXIT_CMD_S) || same_str(input, EXIT_CMD_L)){
        comm->type = EXIT_CMD;
    }
    else {
        // print command
        comm->type = PRINT_CMD;
        char **tokens = tokenize_print_instr(input);
        if (tokens == NULL){
            // invalid instruction
            free(comm);
            return NULL;
        }
        PrintCommand *print = tokens_to_print_comm(tokens);
        if (!print){
            for (int i = 0; i < 3; i++){
                free(tokens[i]);
            }
            free(comm);
            return NULL;
        }
        comm->command.print = print;
    }
    return comm;    
}

#define MAX_REG (16)
#define MAX_MEMORY (65532)

static void execute_print_comm(PrintCommand *comm, CpuState *cpu_state){
    if (!comm){
        return;
    }
    uint32_t target;
    if (comm->is_register_print){
        if ( ! (comm->print_target <= MAX_REG) ){
            printf("Invalid register number\n");
            return;
        }
        target = cpu_state->registers[comm->print_target];
    }
    else {
        // print little endian memory
        if ( ! (comm->print_target <= MAX_MEMORY) || (comm->print_target % 4 != 0) ){
            printf("Invalid memory address\n");
            return;
        }
        target = fetch_big_endian(comm->print_target, cpu_state);
    }
    if (comm->format == BIN){
        //commbits_32(target);
        print_bits_32(target);
    }
    else if (comm->format == HEX) {
        printf("$: 0x%0.8x\n", target);
    }
    else {
        printf("$: %d\n", target);
    }
}

static bool execute_command(ExecutableCommand *comm, CpuState *cpu_state){
    switch (comm->type)  {
        case RUN_CMD:
            puts("Running code");
            break;
        case NEXT_CMD:
            puts("Stepping to next command");
            break;
        case EXIT_CMD:
            puts("Exitting the program");
            return true;
        case PRINT_CMD:
            execute_print_comm(comm->command.print, cpu_state); 
            break; 
    }
    return false;
}


bool get_input_and_execute(CpuState *cpu_state){
    char input[MAX_COMMAND_LEN];
    printf(PROMPT_TXT);
    fgets(input, MAX_COMMAND_LEN, stdin);

    // gets rid of the trailing \n
    input[strlen(input)- 1] = '\0';
    ExecutableCommand *comm = parse(input);
    if (comm == NULL){
        puts("You gave me a wrong command. Type 'help' if you don't know the commands'");
        return get_input_and_execute(cpu_state);
    }
    bool ending = execute_command(comm, cpu_state);
    if (!ending && comm->type != NEXT_CMD && comm->type != RUN_CMD) {
        return get_input_and_execute(cpu_state);
    }
    if (ending){
        return true;
    }
    return false;
}
