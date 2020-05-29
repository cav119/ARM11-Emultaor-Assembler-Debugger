#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "asm_branch_instr.h"
#include "str_to_int_hash_tb.h"



#define EQ_CODE (0)
#define NE_CODE (1)
#define GE_CODE (10)
#define LT_CODE (11)
#define GT_CODE (12)
#define LE_CODE (13)
#define AL_CODE (14)

Dict init_rule_hash(){
    Dict hash_table = dict_create();
    // Inserts the suffixes and corresponding codes
    // for the branch instruction
    dict_insert(hash_table, "eq", EQ_CODE);
    dict_insert(hash_table, "ne", NE_CODE);
    dict_insert(hash_table,"ge", GE_CODE);
    dict_insert(hash_table,"lt", LT_CODE);
    dict_insert(hash_table,"gt", GT_CODE);
    dict_insert(hash_table,"le", LE_CODE);
    dict_insert(hash_table,"al", AL_CODE);
    return hash_table;
}


Instruction *decode_branch_instr_to_bin(char code[5][512]){
    Dict condition_codes = init_rule_hash();
    char *rule = strdup(code[0]);
    if (rule[0] != 'b'){
        // must be a label, not a jump
    }
    else {
        // get the condition
        char cond[2] = {code[0][1], code[0][2], '\0'};
        uint8_t int_code = dict_search(condition_codes, cond);
        // see the label
        char *label = strdup(code[1]);
        //check against list of labels and see whether it's inside

        // if the label isn't found then add 
        // the instruction to the waiting list
        free(label);
    }
    free(rule);
    return 0;
}


