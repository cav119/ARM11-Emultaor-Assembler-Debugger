#include "parser.h"
#include "asm_branch_instr.h"
#include "asm_multiply_instr.h"
#include "asm_single_data_transfer_instr.h"
#include "asm_data_proc_instr.h"
#include "array_list.h"
#include "file_io.h"

#include <assert.h>

#define MAX_WAITING_BRANCHES (50)
#define LINE_SIZE (512)
#define MAX_TOKENS (5)

char **instr_to_tokens(char array[]) {
    char **words = calloc(MAX_TOKENS, LINE_SIZE * sizeof(char));
    char *arr = strtok(array, " ,:\n");
    int i = 0;
    while (arr != NULL) {
        words[i] = strdup(arr);
        arr = strtok(NULL, " ,:\n");
        i ++;
    }
    return words;
}

// Writes the binary to the output file for all the encoded instructions
static void generate_binary_code(List *instructions, FILE *outf) {
    ListNode *curr_instr = instructions->head;
    while (curr_instr != NULL) {
        AsmInstruction *instr = curr_instr->elem;
        write_bytes(outf, *(instr->code));
        curr_instr = curr_instr->next;
    }
}


static bool same_str(const char* str1, const char *str2) {
    return strcmp(str1, str2) == 0;
}

static int str_comp(const void *arg1, const void *arg2) {
    char *str1 = (char *) arg1;
    char *str2 = (char *) arg2;
    return same_str(str1, str2);

}

static void put_instr_to_label(bool *label_next_instr, long curr_number, HashTable *symbol_table,
    ArrayList *labels) {

    if (! (*label_next_instr) ) {
        return;
    }
    *label_next_instr = false;
    long *line = malloc(sizeof(long));
    *line = curr_number;
    for (int i = 0; i < labels->size; i++) {
        WaitingLabel *label = labels->elements[i];
        if (!label->solved) {
            ht_set(symbol_table, label->name, line, hash_str_size(label->name));
            label->solved = true;
        }
    }
}


static void free_waiting_branch(WaitingBranchInstr *waiting) {
    free(waiting->name);
    // do not free instruction code since it is used later
    free(waiting);
}

// between the current execution of an instruction and reaching another one we need
// to subtract 8 to take into account the pipeline execution
#define PC_PIPE_LAG (8)

static void add_offset_to_branch(uint32_t *inst, long branch_line, long target_line) {
    int32_t offset = target_line - branch_line - PC_PIPE_LAG;
    // offset must be shifted right with 2 bits
    offset >>= 2;

    // mask with bits 24-31 equal to 0 
    int32_t offset_mask = 0x00ffffff;
    offset &= offset_mask;
    *inst |= offset;

}

static void add_labels_to_waiting_inst(HashTable *symbol_table, ArrayList *waiting_branches) {
    for (int i = 0; i < waiting_branches->size; i++) {
        WaitingBranchInstr *br_inst = arrlist_get(waiting_branches, i);
        if (br_inst->solved) {
            // label has been solved, no need to check it anymore
            continue;
        }
        char *key = br_inst->name;
        long *label_line = ht_get(symbol_table, key, hash_str_size(key)); 
        // The label's instruction line has been defined
        if (label_line != NULL && *label_line != -1) {
            // found label 
            add_offset_to_branch(br_inst->instruction, br_inst->instr_line, *label_line);
            br_inst->solved = true;
            printf("Found missing label %s which points to instruction 0x%.8x\n", key, *label_line);
        }
    }

}

void decode_instruction(const char *instr[], long *instr_number,
        HashTable *symbol_table, ArrayList *waiting_branches,
        bool *label_next_instr, ArrayList *waiting_labels,
        List *instructions, List *dumped_bytes, List *pending_offset_addrs) {
    
    if (instr[0] == NULL) return;

    AsmInstruction *new_instruction;
    if (same_str(instr[0], "mul") || same_str(instr[0], "mla")) {
        // Multiply instr
        put_instr_to_label(label_next_instr, *instr_number, symbol_table, waiting_labels);
        new_instruction = encode_multiply(instr, instr_number);
        list_append(instructions, new_instruction);
        *instr_number += 4;

    }
    else if (same_str(instr[0], "ldr") || same_str(instr[0], "str")) {
        // Single data transfer instr
        new_instruction = encode_sdt_instr_to_binary(instr, instr_number, dumped_bytes, 
            pending_offset_addrs);
        list_append(instructions, new_instruction);
        *instr_number += 4;
    }
    // branch instrucntion or label
    else if (instr[0][0] == 'b' || instr[1] == NULL) {

        put_instr_to_label(label_next_instr, *instr_number, symbol_table, waiting_labels);
        bool succeeded = false;
        AsmInstruction *branch = encode_branch_instr(instr, symbol_table, waiting_branches ,
                label_next_instr, waiting_labels, instr_number);
      
        if (instr[1] != NULL) {
            // not a label
            list_append(instructions, branch);
            *instr_number += 4;
        }
        else {
            // puts the label in the right place for the instructions with missing labels, if any
            add_labels_to_waiting_inst(symbol_table, waiting_branches);

        }
        // Branch instr
    }
    else {
        // Data processing instr
        put_instr_to_label(label_next_instr, *instr_number, symbol_table, waiting_labels);
        int tokens_size = 0;
        for (tokens_size = 0; tokens_size < MAX_TOKENS; tokens_size++) {
           if (instr[tokens_size] == NULL) {
               break;
           }
        }
        new_instruction = encode_dp_instr_to_binary(instr, tokens_size , instr_number);
        list_append(instructions, new_instruction);
        *instr_number += 4;
    }

}


// compares two WaitingBrachInstr by the label name
static int cmp_waiting_branches(const void *arg1, const void *arg2) {
    if (arg1 == NULL || arg2 == NULL) {
        return 0;
    }
    WaitingBranchInstr *l1 = (WaitingBranchInstr *) arg1;
    WaitingBranchInstr *l2 = (WaitingBranchInstr *) arg2;

    return strcmp(l1->name, l2->name) == 0;
}

static void free_tokens(char **tokens) {
    for (int i = 0; i < MAX_TOKENS; i++) {
        // frees every word since all of them get allocated
        //printf("Just freed : %s\n", tokens[i]);
        free(tokens[i]);
    }
    free(tokens);
}

// function that frees a WaitingLabel
static void free_waiting_label(WaitingLabel *label) {
    if (label == NULL) {
        return;
    }
    if (label->name) {
        free(label->name);
    }
    free(label);
}

static void free_asm_instr(AsmInstruction *asm_inst) {
    if (asm_inst == NULL) {
        return;
    }
    free(asm_inst->code);
    free(asm_inst);
}

// Calculates the pending offsets which have not been set yet
static void calculate_pending_offsets(List *instructions, List *dumped_bytes, 
    List *pending_offset_instr_addrs) {

    int last_instr_idx = instructions->size - 1;
    ListNode *curr_bytes_node = dumped_bytes->head;
    ListNode *curr_addr_node = pending_offset_instr_addrs->head;
    int data_index = 1;

    // both lists dumped_bytes and pending_offset_instr_addrs have the same size
    while (curr_bytes_node != NULL && curr_addr_node != NULL) {
        uint32_t instr_addr = *((uint32_t *)curr_addr_node->elem);
        AsmInstruction *instr = list_get_index(instructions, instr_addr);
        assert(instr != NULL);
        *(instr->code) |= last_instr_idx + data_index - instr_addr;
        AsmInstruction *constant_bytes = malloc(sizeof(AsmInstruction));    // freed later
        constant_bytes->code = curr_bytes_node->elem;
        list_append(instructions, constant_bytes);

        data_index++;
        curr_bytes_node = curr_bytes_node->next;
        curr_addr_node = curr_addr_node->next;
    }
}

// One-pass assembler function (called from main)
void encode_file_lines(FILE* inp_file, FILE* out_file) {

    ArrayList *waiting_labels = arrlist_init();
    ArrayList *waiting_branches = arrlist_init();
    // where the instruction sits in memory
    HashTable *symbol_table = ht_create(str_comp);
    long current_line = 0;

    // checks if a label needs to be found
    bool *next_instr_to_label = malloc(sizeof(bool));
    *next_instr_to_label = false;

    List *instructions = create_list();

    // Helper lists for calculating offsets for LDR instructions
    List *dumped_bytes = create_list();
    List *pending_offset_instr_addrs = create_list();

    // Parse the instructions and generate the binary in one pass
    char buffer[LINE_SIZE];
    while (fgets(buffer, LINE_SIZE, inp_file)) {
        char **words = instr_to_tokens(buffer);
        decode_instruction(words , &current_line, symbol_table,
                waiting_branches, next_instr_to_label, waiting_labels, 
                instructions, dumped_bytes, pending_offset_instr_addrs);
        // frees the tokens after the operations are done 
        free_tokens(words);
    }

    // Setting any remaining labels that haven't been properly set
    add_labels_to_waiting_inst(symbol_table, waiting_branches);

    // Calculate any offsets for LDR instructions that weren't set
    calculate_pending_offsets(instructions, dumped_bytes, pending_offset_instr_addrs);

    // Write the binary code to the output file
    generate_binary_code(instructions, out_file);

    // Free all the allocated resources
    free(next_instr_to_label);
    
    arrlist_destroy_free(waiting_labels, free_waiting_label);
    arrlist_destroy_free(waiting_branches, free_waiting_branch);
    // this free suffices since the HT only has char *keys, and long *values
    // so there is no need for a more advanced free function
    ht_free(symbol_table);
    list_destroy(instructions, free_asm_instr);
    list_destroy(dumped_bytes, free);
    list_destroy(pending_offset_instr_addrs, free);
}

