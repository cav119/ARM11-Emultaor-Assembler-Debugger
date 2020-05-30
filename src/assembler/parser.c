#include "parser.h"
#include "asm_branch_instr.h"
#include "asm_multiply_instr.h"

Instruction *decode_instruction(const char *instr[]){
    if (instr[2] == "\0") {
        return decode_branch_instr_to_bin(instr);
    } else if (instr[0] == "mul" || instr[0] == "mla"){
        return encode_multiply(instr /*,symbolTable*/);
    } else if (instr[0] == "ldr" || instr[0] == "str"){
        return encode_sdt_instr_to_binary(instr);
    } else if (instr[0] == "lsl" || instr[0] == "andeq"){
        return encode_special(instr) ;//special//
    } else{
        return enoce_data_proc(instr);
    }
    
}
