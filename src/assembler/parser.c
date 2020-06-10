#include "parser.h"
#include "asm_branch_instr.h"
#include "asm_multiply_instr.h"
#include "asm_branch_instr.h"
#include "array_list.h"


#define MAX_WAITING_BRANCHES (50)
#define LINE_SIZE (512)
#define MAX_TOKENS (5)

char **instr_to_tokens(char array[]){
    char **words = calloc(MAX_TOKENS, LINE_SIZE * sizeof(char));
    char *arr = strtok(array, " ,:\n");
    int i = 0;
    while (arr != NULL){
        words[i] = strdup(arr);
        arr = strtok(NULL, " ,:\n");
        i ++;
    }
    return words;
}

void generate_binary_code(const char **lines, int num_lines, /* SymbolTable table,*/FILE *outf){
    for (size_t i = 0; i < num_lines; i++){
        //write_bytes(decode_instruction(instr_to_token(lines[i]))->code ,outf);
    }
}


static bool same_str(const char* str1, const char *str2){
    return strcmp(str1, str2) == 0;
}

static int str_comp(const void *arg1, const void *arg2){
    char *str1 = (char *) arg1;
    char *str2 = (char *) arg2;
    return same_str(str1, str2);

}

static void put_instr_to_label(bool *label_next_instr, long curr_number, HashTable *symbol_table, ArrayList *labels){
    if (! (*label_next_instr) ){
        return;
    }
    *label_next_instr = false;
    long *line = malloc(sizeof(long));
    *line = curr_number;
    for (int i = 0; i < labels->size; i++){
        WaitingLabel *label = labels->elements[i];
        if (!label->solved) {
            ht_set(symbol_table, label->name, line, hash_str_size(label->name));
            label->solved = true;
        }
    }
}


static void free_waiting_branch(WaitingBranchInstr *waiting){
    free(waiting->name);
    // do not free instruction code since it is used later
    free(waiting);
}

// between the current execution of an instruction and reaching another one we need
// to subtract 8 to take into account the pipeline execution
#define PC_PIPE_LAG (8)

static void add_offset_to_branch(uint32_t *inst, long branch_line, long target_line){
    int32_t offset = target_line - branch_line - PC_PIPE_LAG;
    // offset must be shifted right with 2 bits
    offset >>= 2;

    // mask with bits 24-31 equal to 0 
    int32_t offset_mask = 0x00ffffff;
    offset &= offset_mask;
    *inst |= offset;

}

static void add_labels_to_waiting_inst(HashTable *symbol_table, ArrayList *waiting_branches){
    for (int i = 0; i < waiting_branches->size; i++){
        WaitingBranchInstr *br_inst = arrlist_get(waiting_branches, i);
        if (br_inst->solved){
            // label has been solved, no need to check it anymore
            continue;
        }
        char *key = br_inst->name;
        long *label_line = ht_get(symbol_table, key, hash_str_size(key)); 
        // The label's instruction line has been defined
        if (label_line != NULL && *label_line != -1){
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
        List *instructions){

    if (same_str(instr[0], "mul") || same_str(instr[0], "mla")){
        // Multiply instr
        put_instr_to_label(label_next_instr, *instr_number, symbol_table, waiting_labels);
        *instr_number += 4;

    }
    else if (same_str(instr[0], "ldr") || same_str(instr[0], "str")){
        // Single data transfer instr
        put_instr_to_label(label_next_instr, *instr_number, symbol_table, waiting_labels);
        *instr_number += 4;
    }
    else if (same_str(instr[0], "lsl") || same_str(instr[0], "andeq")){
        // Special
        *instr_number += 4;

    }
    // branch instrucntion or label
    else if (instr[0][0] == 'b' || instr[1] == NULL){

        put_instr_to_label(label_next_instr, *instr_number, symbol_table, waiting_labels);
        bool succeeded = false;
        AsmInstruction *branch = encode_branch_instr(instr, symbol_table, waiting_branches ,
                label_next_instr, waiting_labels, instr_number);
      
        if (instr[1] != NULL){
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
        *instr_number += 4;
    }

}


// compares two WaitingBrachInstr by the label name
static int cmp_waiting_branches(const void *arg1, const void *arg2){
    if (arg1 == NULL || arg2 == NULL){
        return 0;
    }
    WaitingBranchInstr *l1 = (WaitingBranchInstr *) arg1;
    WaitingBranchInstr *l2 = (WaitingBranchInstr *) arg2;

    return strcmp(l1->name, l2->name) == 0;
}

static void free_tokens(char **tokens){
    for (int i = 0; i < MAX_TOKENS; i++){
        // frees every word since all of them get allocated
        printf("Just freed : %s\n", tokens[i]);
        free(tokens[i]);
    }
    free(tokens);
}

// function that frees a WaitingLabel
static void free_waiting_label(WaitingLabel *label){
    if (label == NULL){
        return;
    }
    if (label->name){
        free(label->name);
    }
    free(label);
}

static void free_asm_instr(AsmInstruction *asm_inst){
    if (asm_inst == NULL){
        return;
    }
    free(asm_inst->code);
    free(asm_inst);
}

// Function that gets called from main
void encode_file_lines(FILE* fp){

    //char **array_of_words[nlines];
    //uint32_t *instructions = malloc(nlines * sizeof(uint32_t));
    ArrayList *waiting_labels = arrlist_init();
    ArrayList *waiting_branches = arrlist_init();
    // where the instruction sits in memory
    HashTable *symbol_table = ht_create(str_comp);
    long current_line = 0;

    // checks if a label needs to be found
    bool *next_instr_to_label = malloc(sizeof(bool));
    *next_instr_to_label = false;

    List *instructions = create_list();

    // gets to parsing
    char buffer[LINE_SIZE];
    while (fgets(buffer, LINE_SIZE, fp)){
        char **words = instr_to_tokens(buffer);
        decode_instruction(words , &current_line, symbol_table,
                waiting_branches, next_instr_to_label, waiting_labels, instructions);
        // frees the tokens after the operations are done 
        free_tokens(words);
    }

    // Setting any remaining labels that haven't been properly set
    add_labels_to_waiting_inst(symbol_table, waiting_branches);


    free(next_instr_to_label);
    
    arrlist_destroy_free(waiting_labels, free_waiting_label);
    arrlist_destroy_free(waiting_branches, free_waiting_branch);
    // this free suffices since the HT only has char *keys, and long *values
    // so there is no need for a more advanced free function
    ht_free(symbol_table);
    list_destroy(instructions, free_asm_instr); 
}


