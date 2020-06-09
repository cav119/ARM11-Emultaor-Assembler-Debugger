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
    size_t code_str_size = sizeof(char) * 2;

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

    ht_set(hash_table, str_clone("eq") ,eq_ptr, code_str_size);
    ht_set(hash_table, str_clone("ne"), ne_ptr, code_str_size);
    ht_set(hash_table, str_clone("ge"), ge_ptr, code_str_size);
    ht_set(hash_table, str_clone("lt"), lt_ptr, code_str_size);
    ht_set(hash_table, str_clone("gt"), gt_ptr, code_str_size);
    ht_set(hash_table, str_clone("le"), le_ptr, code_str_size);
    ht_set(hash_table, str_clone("al"), al_ptr, code_str_size);
    return hash_table;
}


AsmInstruction *encode_branch_instr(char **code, HashTable *symbol_table
                , WaitingBranchInstr **waiting_branches, int *waiting_br_size, bool *label_next_instr,
                              char *waiting_label, long *instr_line){ 
    AsmInstruction *asm_instr = calloc(1, sizeof(AsmInstruction));
    uint32_t *instr = malloc(UI32);
    asm_instr->code = instr;
    asm_instr->instr_line = *instr_line; 
    HashTable *codes_maps = init_rule_hash();
    if (code[1] == NULL){
        // must be a label, not a jump
        char *label = str_clone(code[0]);
        
        // the line where the label points
        long *pointing_line = malloc(sizeof(long));
        // so that we know the label's instr line is not known yet 
        *pointing_line = -1;

        *label_next_instr = true;
        ht_set(symbol_table, label, pointing_line, hash_str_size(label));
        strcpy(waiting_label, label);
        ht_free(codes_maps);
        return NULL;
    }
    else {
        // get the condition
        uint32_t int_code;
        if (code[0][1] != '\0') {
            char cond[2] = {code[0][1], code[0][2], '\0'};

            int_code = *((int *) ht_get(codes_maps, cond, sizeof(char) * 2));
        }
        else {
            // always branch
            int_code = AL_CODE;
        }
        // see the label
        char *label = str_clone(code[1]);
        //check against list of labels and see whether it's inside
        *instr = int_code << 28;
        *instr = *instr | (10 << 24); // 1010 at bits 27-24
        long *target = ht_get(symbol_table, label, hash_str_size(label));
        if (target != NULL && *target != -1) {
            // calculate offset according to current line number
            // and offset line number and subtract 8
            int32_t offset = (int32_t) (*target) - *instr_line - 8;
            offset >>= 2;
            int32_t offset_mask = 0x00ffffff;
            offset &= offset_mask;
            *instr |= offset;
        }
        else {
            WaitingBranchInstr *wait_br = malloc(sizeof(WaitingBranchInstr));
            wait_br->name = str_clone(label);
            wait_br->instruction = instr;
            wait_br->instr_line = *instr_line; 
            wait_br->solved = false;
            waiting_branches[*waiting_br_size] = wait_br;
            *waiting_br_size = *waiting_br_size + 1;
            // add wait_br to the waiting instruction list;
        }
        free(label);
    }
    ht_free(codes_maps);
    return asm_instr;
}


