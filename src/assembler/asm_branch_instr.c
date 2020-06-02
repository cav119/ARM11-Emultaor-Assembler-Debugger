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
#define UI32 (sizeof(uint32_t))

// comprator function for hash, required by dict_create
static int compare_str(const void *s1, const void *s2){
    char *str1 = (char *) s1;
    char *str2 = (char *) s2;
    return strcmp(str1, str2) == 0;
}



static HashTable *init_rule_hash(){
    HashTable *hash_table = ht_create(compare_str);
    // Inserts the suffixes and corresponding codes
    // for the branch instruction
    size_t code_str_size = sizeof(char) * 3;

    uint32_t *eq_ptr = malloc(UI32);
    *eq_ptr = EQ_CODE;

    uint32_t *ne_ptr = malloc(UI32);
    *ne_ptr = NE_CODE;

    uint32_t *ge_ptr = malloc(UI32);
    *ge_ptr = GE_CODE;

    uint32_t *lt_ptr = malloc(UI32);
    *lt_ptr = LT_CODE;

    uint32_t *gt_ptr = malloc(UI32);
    *gt_ptr = GT_CODE;

    uint32_t *le_ptr = malloc(UI32);
    *le_ptr = LE_CODE;

    uint32_t *al_ptr = malloc(UI32);
    *al_ptr = AL_CODE;

    ht_set(hash_table, strdup("eq") ,eq_ptr, UI32);
    ht_set(hash_table, strdup("ne"), ne_ptr, UI32);
    ht_set(hash_table, strdup("ge"), ge_ptr, UI32);
    ht_set(hash_table, strdup("lt"), lt_ptr, UI32);
    ht_set(hash_table, strdup("gt"), gt_ptr, UI32);
    ht_set(hash_table, strdup("le"), le_ptr, UI32);
    ht_set(hash_table, strdup("al"), al_ptr, UI32);
    return hash_table;
}


uint32_t *decode_branch_instr_to_bin(char code[5][512], HashTable *label_table
                , WaitingBranchInstr **waiting_branch_instr, int *waiting_br_size, int current_line, bool *label_next_instr,
                char *waiting_label){
    uint32_t *instr = malloc(UI32);
    HashTable *codes_maps = init_rule_hash();
    char *rule = strdup(code[0]);
    if (rule[0] != 'b'){
        // must be a label, not a jump
        char *label = strdup(code[0]);
        
        // the line where the label points
        long *pointing_line = calloc(1, sizeof(long));
        *label_next_instr = true;
        ht_set(label_table, label, pointing_line, hash_str_size(label));
        strcpy(waiting_label, label);

    }
    else {
        // get the condition
        char cond[2] = {code[0][1], code[0][2], '\0'};
        uint32_t *int_code = *( (int *)ht_get(codes_maps, cond, sizeof(char) * 2) );
        // see the label
        char *label = strdup(code[1]);
        //check against list of labels and see whether it's inside
        *instr = *int_code << 28;
        *instr = *instr | (10 << 24); // 1010 at bits 27-24
        if (label_table != NULL && ht_get(label_table, label, hash_str_size(label))){
            // calculate offset according to current line number
            // and offset line number and subtract 8 
        }
        else {
            WaitingBranchInstr *wait_br = malloc(sizeof(WaitingBranchInstr));
            wait_br->name = label;
            wait_br->instruction = instr;
            waiting_branch_instr[*waiting_br_size] = wait_br;
            *waiting_br_size = *waiting_br_size + 1;
            // add wait_br to the waiting instruction list;
        }
    }
    ht_free(codes_maps);
    free(rule);
    return 0;
}


