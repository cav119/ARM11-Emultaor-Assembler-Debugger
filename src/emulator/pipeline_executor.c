#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "utilities.h"
#include "pipeline_executor.h"
#include "pipeline_data.h"
#include "../assembler/hash_table.h"
#include "../extension/command_parser.h"

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


bool execute(Instruction *instruction, CpuState *cpu_state, Pipe *pipe) {
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

#define PIPE_LAG (8)
// Internal helper recursive function for the pipeline execution, makes the code
// cleaner and as efficient using gcc's tail call optimisation
static void start_pipeline_helper(CpuState *cpu_state, Pipe *pipe, bool is_extension, bool is_stepping, HashTable *ht) {
    if (pipe->fetching) {
        pipe->executing = pipe->decoding;
        pipe->decoding = decode_instruction(pipe->fetching);
        if(is_extension){
            int *current_line = malloc(sizeof(int));
            *current_line = cpu_state->registers[PC] - PIPE_LAG;
            if (ht_get(ht,current_line, sizeof(uint32_t) / sizeof(char))){
                printf("reached breakpoint at line 0x%.8x", *current_line);
                is_stepping = true;
            }
            free(current_line);
        }

        bool branch_instr_succeeded = false;
        if (is_extension && is_stepping && get_input_and_execute(cpu_state, &is_stepping, ht)){
            // must have hit the exit command
            // need to free pipe before aborting
            if (pipe->executing){
                free(pipe->executing);
            }
            if (pipe->decoding){
                free(pipe->decoding);
            }
            free(pipe);
            return;
        }
        if (pipe->executing) {
            instruction_type type = pipe->executing->type;
            bool succeeded = execute(pipe->executing,cpu_state, pipe);
            if (succeeded && type == BRANCH) {
                branch_instr_succeeded = true;
            }
            // execute() should also print the effect of the instruction to stdout if
            // the running program is an extension
        }
        else {
            EXTENSION_WRITE(is_extension, "No command is being executed at the moment");
        }
        if (!branch_instr_succeeded) {
            pipe->fetching = fetch(cpu_state->registers[PC], cpu_state);
            increment_pc(cpu_state);
        }
        // Ask user for input
        start_pipeline_helper(cpu_state, pipe, is_extension, is_stepping, ht);
    } else {
        bool ended = end_pipeline(pipe, cpu_state, is_extension, &is_stepping, ht);
        if (!ended) {
            start_pipeline_helper(cpu_state, pipe, is_extension, is_stepping, ht);
        }
    }
}
static int compare_address(const void *b1, const void *b2){
    uint32_t *break_1 = (uint32_t *) b1;
    uint32_t *break_2 = (uint32_t *) b2;
    int compare;
    if (!break_1 || !break_2){
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


void start_pipeline(CpuState *cpu_state, bool is_extension) {
    HashTable  *hash_table = ht_create(compare_address);
    bool *is_stepping = malloc(sizeof(bool));
    *is_stepping = true;
    if(is_extension){
        puts("please type <b> <MEMORY_LOCATION> to add a breakpoint and/or type r to run");
        fflush(stdin);
        //get_input_and_execute(cpu_state, is_stepping, hash_table);
    }
   start_pipeline_helper(cpu_state, init_pipeline(cpu_state), is_extension, is_stepping, hash_table);
}


bool end_pipeline(Pipe *pipe, CpuState *cpu_state, bool is_extension, bool *is_stepping, HashTable *hash_table) {
    // Must have fetched a halt
    // since it stops when fetching a halt the block of code simulates executing 2 cycles
    // first executing pipe->executing, and then pipe->decoding
    if (pipe->executing != NULL) {
        // fetched a HALT, must execute executing and then decoding
        instruction_type type = pipe->executing->type;
        bool succeeded = execute(pipe->executing, cpu_state, pipe);
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
            instruction_type type = pipe->decoding->type;
            bool succeeded = execute(pipe->decoding, cpu_state, pipe);
            if (type == BRANCH && succeeded) {
                return false;
            }
            pipe->decoding = NULL;
        }
    }

    increment_pc(cpu_state);
    free(pipe);
  // free(is_stepping);
  //free(is_extension);
    ht_free(hash_table);

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
