#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "asm_branch_instr.h"
#include "hash_table.h"



#define EQ_CODE (0)
#define NE_CODE (1)
#define GE_CODE (10)
#define LT_CODE (11)
#define GT_CODE (12)
#define LE_CODE (13)

#define AL_CODE (14)

// comprator function for hash, required by dict_create
static uint32_t compare_str(const void *s1, const void *s2){
    char *str1 = (char *) s1;
    char *str2 = (char *) s2;
    return strcmp(str1, str2) == 0;
}

static Dict init_rule_hash(){
    Dict hash_table = dict_create(compare_str);
    // Inserts the suffixes and corresponding codes
    // for the branch instruction
    uint32_t *eq_ptr = malloc(sizeof(uint32_t *));
    *eq_ptr = EQ_CODE;

    uint32_t *ne_ptr = malloc(sizeof(uint32_t *));
    *ne_ptr = NE_CODE;


    uint32_t *ge_ptr = malloc(sizeof(uint32_t *));
    *ge_ptr = GE_CODE;

    uint32_t *lt_ptr = malloc(sizeof(uint32_t *));
    *lt_ptr = LT_CODE;

    uint32_t *gt_ptr = malloc(sizeof(uint32_t *));
    *gt_ptr = GT_CODE;

    uint32_t *le_ptr = malloc(sizeof(uint32_t *));
    *le_ptr = LE_CODE;

    uint32_t *al_ptr = malloc(sizeof(uint32_t *));
    *al_ptr = AL_CODE;

    dict_insert(hash_table, strdup("eq") ,eq_ptr);
    dict_insert(hash_table, strdup("ne"), ne_ptr);
    dict_insert(hash_table, strdup("ge"), ge_ptr);
    dict_insert(hash_table, strdup("lt"), lt_ptr);
    dict_insert(hash_table, strdup("gt"), gt_ptr);
    dict_insert(hash_table, strdup("le"), le_ptr);
    dict_insert(hash_table, strdup("al"), al_ptr);
    return hash_table;
}


Instruction *decode_branch_instr_to_bin(char code[5][512], Dict label_table
                , Dict waiting_labels, int current_line){
    Dict codes_maps = init_rule_hash();
    char *rule = strdup(code[0]);
    if (rule[0] != 'b'){
        // must be a label, not a jump
        char *label = strdup(code[0]);
        /*
            inserts label to the table
            or alternatively sends a message to the caller 
            that we hit a label and the next valid instruction that is reached
            should be the pointer, which would be way better
        if (label_table != NULL){
            dict_insert(label_table, strdup(label), current_line+4); 
        }

        */
        free(label);

    }
    else {
        // get the condition
        char cond[2] = {code[0][1], code[0][2], '\0'};
        uint32_t int_code = *( (int *)dict_search(codes_maps, cond) );
        // see the label
        char *label = strdup(code[1]);
        //check against list of labels and see whether it's inside
        uint32_t instr = int_code << 28;
        instr |= 10 << 24; // 1010 at bits 27-24
        if (label_table != NULL && dict_search(label_table, label)){
            // calculate offset according to current line number
            // and offset line number and subtract 8 
        }
        else {
            if (waiting_labels != NULL){
                // adds to waiting labels the name of the label and the 
                // current line  
            }
        }
        free(label);
    }
    dict_destroy(codes_maps);
    free(rule);
    return 0;
}


