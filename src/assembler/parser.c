#include "parser.h"
#include "asm_branch_instr.h"
#include "asm_multiply_instr.h"


char **instr_to_tokens(char array[]){
    char **words = calloc(5, 512);
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

Instruction *decode_instruction(const char *instr[], long *instr_number,
                                HashTable *symbol_table, HashTable *waiting_branches){
/*    if (instr[2] == "\0") {
        return decode_branch_instr_to_bin(instr);
*/
    if (same_str(instr[0], "mul") || same_str(instr[0], "mla")){
        *instr_number += 4;
        // Multiply instr
    }
    else if (same_str(instr[0], "ldr") || same_str(instr[0], "str")){
        // Single data transfer instr
        *instr_number += 4;
    }
    else if (same_str(instr[0], "lsl") || same_str(instr[0], "andeq")){
        // Special
        *instr_number += 4;

    }
    else if (instr[0][0] == 'b' || instr[1] == NULL){
        // Branch instr
        
    }
    else {
        // Data processing instr
        *instr_number += 4;
    }
    return 0;
     
}

void encode_file_lines(char **lines, size_t nlines){
    char **array_of_words[nlines];

    // where the instruction sits in memory
    long current_line = 0;
    for (int i = 0; i < nlines; i++) {
      array_of_words[i] = instr_to_tokens(lines[i]);
    }

    for (int i = 0; i < nlines; i++) {
      for (int j = 0; j < 5; j++) {
        printf("j = %d , %s\n", j, array_of_words[i][j]);
      }
      printf("\n");
    }
}
