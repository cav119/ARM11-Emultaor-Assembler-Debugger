#include "parser.h"
#include "asm_branch_instr.h"
#include "asm_multiply_instr.h"

#define MAX_WAITING_BRANCHES (50)
#define LINE_SIZE (512)

char **instr_to_tokens(char array[]){
    char **words = calloc(5, LINE_SIZE);
    char *arr = strtok(array, " ,:\n");
    int i = 0;
    while (arr != NULL){
        words[i] = arr;
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

void put_instr_to_label(bool *label_next_instr, long curr_number, HashTable *symbol_table, char *label){
    if (! (*label_next_instr) ){
        return;
    }
    *label_next_instr = false;
    long *line = malloc(sizeof(long));
    *line = curr_number;
    ht_set(symbol_table, label, line, hash_str_size(label));
}

uint32_t *decode_instruction(const char *instr[], long *instr_number,
                                HashTable *symbol_table, WaitingBranchInstr *waiting_branches,
                                int *wait_br_size, bool *label_next_instr, char *waiting_label){
/*    if (instr[2] == "\0") {
        return decode_branch_instr_to_bin(instr);
*/

    if (same_str(instr[0], "mul") || same_str(instr[0], "mla")){
        // Multiply instr
        put_instr_to_label(label_next_instr, instr_number, symbol_table, waiting_label);
        *instr_number += 4;

    }
    else if (same_str(instr[0], "ldr") || same_str(instr[0], "str")){
        // Single data transfer instr
        put_instr_to_label(label_next_instr, instr_number, symbol_table, waiting_label);
        *instr_number += 4;
    }
    else if (same_str(instr[0], "lsl") || same_str(instr[0], "andeq")){
        // Special
        *instr_number += 4;

    }
    else if (instr[0][0] == 'b' || instr[1] == NULL){
        put_instr_to_label(label_next_instr, instr_number, symbol_table, waiting_label);
        // Branch instr
    }
    else {
        // Data processing instr
        put_instr_to_label(label_next_instr, instr_number, symbol_table, waiting_label);
        *instr_number += 4;
    }
    return 0;
     
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

void encode_file_lines(char **lines, size_t nlines){
    char **array_of_words[nlines];
    char *waiting_label = malloc(sizeof(char) * LINE_SIZE);
    WaitingBranchInstr *waiting_branches = calloc(MAX_WAITING_BRANCHES, sizeof(WaitingBranchInstr));

    int *waiting_br_size = malloc(sizeof(int));
    *waiting_br_size = 0;
    // where the instruction sits in memory
    HashTable *symbol_table = ht_create(str_comp);
    long current_line = 0;

    // checks if a label needs to be found
    bool *next_instr_to_label = malloc(sizeof(bool));
    *next_instr_to_label = false;
    for (int i = 0; i < nlines; i++) {
      array_of_words[i] = instr_to_tokens(lines[i]);
      uint32_t *instr = decode_instruction(array_of_words[i], &current_line, symbol_table,
                        waiting_branches, waiting_br_size, next_instr_to_label, waiting_label);
    }

    for (int i = 0; i < nlines; i++) {
      for (int j = 0; j < 5; j++) {
        printf("j = %d , %s\n", j, array_of_words[i][j]);
      }
      printf("\n");
    }
}


