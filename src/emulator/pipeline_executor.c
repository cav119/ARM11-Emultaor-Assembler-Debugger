#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "utilities.h"
#include "pipeline_executor.h"
#include "pipeline_data.h"

#include "../../extension/command_parser.h"
#include "../../extension/gui.h"

#define PIPE_LAG (8)


// If running the extension, output this message to stdout
#define EXTENSION_WRITE(is_ext, msg) \
    do { if (is_ext) { puts(msg); } } while (0)

Pipe *init_pipeline(CpuState *cpu_state) {
    Pipe *pipe = calloc(1, sizeof(Pipe));
    pipe->fetching = fetch(0, cpu_state);
    increment_pc(cpu_state);

    return pipe;
}


uint32_t fetch(uint32_t ptr, CpuState *cpu_state) {
    bool valid = check_valid_memory_access(cpu_state, ptr);
    if (!valid) {
        return 0;
    }

    return index_little_endian_bytes(&(cpu_state->memory[ptr]));
}

uint32_t fetch_big_endian(uint32_t ptr, CpuState *cpu_state) {
    bool valid = check_valid_memory_access(cpu_state, ptr);
    if (!valid) {
        return 0;
    }

    return index_big_endian_bytes(&(cpu_state->memory[ptr]));
}


bool is_branch_instr(uint32_t bits) {
    // bits 27-24  are 1010
    return process_mask(bits, 24, 27) == 10;
}


bool is_multiply_instr(uint32_t bits) {
    // bits 27-22  are all 0
    bool top_bits_zero = process_mask(bits, 22, 27) == 0;
    // bits 7-4  are 1001
    bool lower_bits = process_mask(bits, 4, 7) == 9;
    return top_bits_zero && lower_bits;
}


bool is_single_data_transfer_instr(uint32_t bits) {
    // bits 27-26 are 01init_pipeline
    return process_mask(bits, 26, 27) == 1;
}


Instruction *decode_instruction(uint32_t bits) {
    Instruction *instr = malloc(sizeof(Instruction));
    if (is_branch_instr(bits)){
        instr->type = BRANCH;
    }
    else if (is_multiply_instr(bits)){
        instr->type = MULTIPLY;
    }
    else if (is_single_data_transfer_instr(bits)){
        instr->type = SINGLE_DATA_TRANSFER;
    }
    else {
        instr->type = DATA_PROCESS;
    }
    instr->code = bits;
    return instr;
}


bool execute(Instruction *instruction, CpuState *cpu_state, Pipe *pipe, MainWin *win) {
    if (!(check_CPSR_cond(process_mask(instruction->code, 28, 31), cpu_state))) {
        free(instruction);
        return false;
    }

    switch (instruction->type) {
        case DATA_PROCESS:
            execute_data_processing_instr(cpu_state, instruction);
            free(instruction);
            pipe->executing = 0x0;
            break;
        case MULTIPLY:
            execute_multiply_instruction(instruction, cpu_state);
            pipe->executing = 0x0;
            free(instruction);
            break;
        case SINGLE_DATA_TRANSFER:
            execute_single_data_transfer_instr(cpu_state, instruction);
            free(instruction);
            pipe->executing = 0x0;
            break;
        case BRANCH:
            // no need to free instruction because branch handles it
            return execute_branch_instr(instruction, cpu_state, pipe);
    }

    return true;
}


static void check_breakpoints(CpuState *cpu_state, bool is_extension, bool *is_stepping, 
    HashTable *breakpoint_map, MainWin *win) {
    if(is_extension){
        int *current_line = malloc(sizeof(int));
        *current_line = cpu_state->registers[PC] - PIPE_LAG;
        
        if (ht_get(breakpoint_map,current_line, sizeof(uint32_t) / sizeof(char))) {
            char *msg = malloc(sizeof(char) * win->out_win->win->_maxx - 3);
            sprintf(msg, "Hit breakpoint at address 0x%.8x\n", *current_line);
            print_to_output(win->out_win, msg);
            wmove(win->inp_win->win, 1, 1 + PROMPT_SIZE);
            
            *is_stepping = true;
        }
        free(current_line);
    }
}

static void free_pipe(Pipe *pipe){
    if (pipe->executing){
        free(pipe->executing);
    }
    if (pipe->decoding){
        free(pipe->decoding);
    }
    free(pipe);
}

// Internal helper recursive function for the pipeline execution, makes the code
// cleaner and as efficient using gcc's tail call optimisation
static void start_pipeline_helper(CpuState *cpu_state, Pipe *pipe, bool is_extension, 
    bool *is_stepping, HashTable *breakpoint_map, MainWin *win) {
    if (pipe->fetching) {
        pipe->executing = pipe->decoding;
        pipe->decoding = decode_instruction(pipe->fetching);
        check_breakpoints(cpu_state, is_extension, is_stepping, breakpoint_map, win);
        bool branch_instr_succeeded = false;
        if (is_extension && *is_stepping && get_input_and_execute(cpu_state, is_stepping, breakpoint_map, win)){
            // must have hit the exit command
            // need to free pipe before aborting
            free_pipe(pipe);
            ht_free(breakpoint_map);
            free(is_stepping);
            return;
        }
        if (pipe->executing) {
            instruction_type type = pipe->executing->type;
            bool succeeded = execute(pipe->executing,cpu_state, pipe, win);
            if (succeeded && type == BRANCH) {
                branch_instr_succeeded = true;
            }
            // execute() should also print the effect of the instruction to stdout if
            // the running program is an extension
        }
        if (!branch_instr_succeeded) {
            pipe->fetching = fetch(cpu_state->registers[PC], cpu_state);
            increment_pc(cpu_state);

            if (is_extension) {
                update_memory_map_by_word(win->memory_win, cpu_state->memory, cpu_state->registers[PC] - PIPE_LAG);
                update_registers(win->regs_win, cpu_state->registers);
                wrefresh(win->regs_win->win);
            }
        }
        // Ask user for input
        start_pipeline_helper(cpu_state, pipe, is_extension, is_stepping, breakpoint_map, win);
    } else {
        bool ended = end_pipeline(pipe, cpu_state, is_extension, is_stepping, breakpoint_map, win);
        if (!ended) {
            start_pipeline_helper(cpu_state, pipe, is_extension, is_stepping, breakpoint_map, win);
        } else if (is_extension) {
            update_memory_map_by_word(win->memory_win, cpu_state->memory, cpu_state->registers[PC] - PIPE_LAG);
        }
    }
}

static int compare_address(const void *b1, const void *b2){
    uint32_t *break_1 = (uint32_t *) b1;
    uint32_t *break_2 = (uint32_t *) b2;
    int compare;
    if (!break_1 || !break_2) {
        perror("null breakpoints");
        exit(EXIT_FAILURE);
    }
    if (break_1 > break_2){
        compare = 1;
    } else if (break_1 < break_2){
        compare = -1;
    } else {
        compare = 0;
    }
    return compare;
}


void start_pipeline(CpuState *cpu_state, bool is_extension, MainWin *win) {
    HashTable  *breakpoint_map = ht_create(compare_address);
    bool *is_stepping = malloc(sizeof(bool));
    *is_stepping = true;
    if (is_extension) {
        // Load the memory and display it when the program is loaded
        print_to_output(win->out_win, "Loaded program. Type 'r' or 'run' to start the execution.");
        update_memory_map_by_word(win->memory_win, cpu_state->memory, 0);
        wmove(win->inp_win->win, 1, 1 + PROMPT_SIZE);
    }
    start_pipeline_helper(cpu_state, init_pipeline(cpu_state), is_extension, is_stepping, breakpoint_map, win);
}


bool end_pipeline(Pipe *pipe, CpuState *cpu_state, bool is_extension, bool *is_stepping, HashTable *breakpoint_map, MainWin *win) {
    // Must have fetched a halt
    // since it stops when fetching a halt the block of code simulates executing 2 cycles
    // first executing pipe->executing, and then pipe->decoding
    if (pipe->executing != NULL) {
        // fetched a HALT, must execute executing and then decoding
        check_breakpoints(cpu_state, is_extension, is_stepping, breakpoint_map, win);
        if (is_extension && *is_stepping && get_input_and_execute(cpu_state, is_stepping, breakpoint_map, win)) {
            free_pipe(pipe);
            ht_free(breakpoint_map);
            free(is_stepping);
            return true;
        }
        instruction_type type = pipe->executing->type;
        bool succeeded = execute(pipe->executing, cpu_state, pipe, win);
        if (type == BRANCH && succeeded) {
            return false;
        }
        increment_pc(cpu_state);
        if (pipe->decoding != NULL){
            free(pipe->decoding);
            pipe->decoding = NULL;
        }
    } else {
        if (pipe->decoding != NULL) {
            check_breakpoints(cpu_state, is_extension, is_stepping, breakpoint_map, win);
            if (is_extension && *is_stepping && get_input_and_execute(cpu_state, is_stepping, breakpoint_map, win)) {
               free_pipe(pipe);
               ht_free(breakpoint_map);
               free(is_stepping);
               return true;
            }
            instruction_type type = pipe->decoding->type;
            bool succeeded = execute(pipe->decoding, cpu_state, pipe, win);
            if (type == BRANCH && succeeded) {
                return false;
            }
            pipe->decoding = NULL;
        }
    }

    increment_pc(cpu_state);
    free(pipe);
    ht_free(breakpoint_map);
    free(is_stepping);

    return true;
}


void clear_pipe(Pipe *pipe){
    // clears the whole pipeline after a branch inst
    if (pipe->executing != NULL){
        free(pipe->executing);
    }
    if (pipe->decoding != NULL){
        free(pipe->decoding);
    }
    pipe->executing = 0x0;
    pipe->decoding = 0x0;
    pipe->fetching = 0x0;
}


void print_pipeline(Pipe *pipe){
    printf("%.8x\n", pipe->executing ? pipe->executing->code : 0);
    printf("%.8x\n", pipe->decoding ? pipe->decoding->code : 0);
    printf("%.8x\n", pipe->fetching);
}
