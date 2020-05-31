#include "parser.h"
#include "asm_branch_instr.h"
#include "asm_multiply_instr.h"


char **instr_to_token(char array[]){
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

void generate_binary_code(const char **lines, int num_lines, /* SymbolTable table,*/ FILE *outf){
    for (size_t i = 0; i < num_lines; i++){
        write_bytes(decode_instruction(instr_to_token(lines[i]))->code ,outf);
    }
}

Instruction *decode_instruction(const char *instr[]){
    if (instr[2] == "\0") {
      //  return decode_branch_instr_to_bin(instr);
    } else if (instr[0] == "mul" || instr[0] == "mla"){
       // return encode_multiply(instr /*,symbolTable*/);
    } else if (instr[0] == "ldr" || instr[0] == "str"){
     //   return encode_sdt_instr_to_binary(instr);
    } else if (instr[0] == "lsl" || instr[0] == "andeq"){
       // return encode_special(instr) ;//special//
    } else{
     //   return enoce_data_proc(instr);
    }
    return 0;
    
}
