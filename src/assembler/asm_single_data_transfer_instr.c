#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "asm_single_data_transfer_instr.h"
#include "asm_data_proc_instr.h"

#include "asm_utilities.h"


// Internal helper functions for setting the bits of the binary code
static void set_ldr_str_bit(const char *token, uint32_t *bin_code);
static void set_transfer_reg_bits(char *token, uint32_t *bin_code);
static bool set_address_bits(char *tokens[], uint32_t *bin_code, uint32_t curr_instr, 
    List *dumped_bytes_list, List *pending_offset_instr_addrs);


AsmInstruction *encode_sdt_instr_to_binary(char *tokens[], long *curr_instr,
    List *dumped_bytes_list, List *pending_offset_instr_addrs) {

    AsmInstruction *inst = calloc(1, sizeof(AsmInstruction));
    if (!inst) {
        perror("Error allocating memory for asm instruction");
        exit(EXIT_FAILURE);
    }
    inst->instr_line = *curr_instr;
    // set condition code, up bit and 01 format bits
    uint32_t bin_code = 0xE << 28 | 1 << 26 | 1 << 23;

    set_ldr_str_bit(tokens[0], &bin_code);
    set_transfer_reg_bits(tokens[1], &bin_code);
    bool not_mov = set_address_bits(tokens, &bin_code, *curr_instr, dumped_bytes_list, 
        pending_offset_instr_addrs);

    if (!not_mov) {
        // call to encode_data_proc_inst instead (treated as a mov instr)
        int tokens_size = 0;
        for (tokens_size = 0; tokens_size < 5; tokens_size++){
            if (tokens[tokens_size] == NULL){
                break;
            }
        }
        free(inst);

        tokens[2][0] = '#';
        char *mov_instr[] = {"mov", tokens[1], tokens[2]};
        return encode_dp_instr_to_binary(mov_instr, tokens_size, curr_instr);
    }
    uint32_t *actual_code = malloc(sizeof(uint32_t));
    if (!actual_code) {
        perror("Error allocating memory for actual_code in single_data_trans");
        exit(EXIT_FAILURE);
    }
    *actual_code = bin_code;
    inst->code = actual_code;
    return inst;
}


// Sets L bit depending if token is ldr or str in the binary code
void set_ldr_str_bit(const char *token, uint32_t *bin_code) {
    if (strcmp(token, "ldr") == 0) {
        *bin_code |= 1 << 20; // set bit 20
    }
}


// Sets the transfer register (Rd) bits (15-12) in the binary code
void set_transfer_reg_bits(char *token, uint32_t *bin_code) {
    char reg_num = read_reg_num(token);
    *bin_code |= reg_num << 12; // set bits 15-12 for Rd
}



////////// ADDRESSING ///////////


// Returns true if address is a numeric constant, false if indexed
bool is_constant_address(const char *token) {
    if (token[0] == '=') {
        return true;
    }

    return false;
}


// Gets the numeric value from =0x12345
uint32_t parse_constant_address(char *token) {
    int base = token[2] == 'x' ? 16 : 10;

    char address_str[strlen(token) - 1];
    memcpy(address_str, &token[1], strlen(token));
    address_str[strlen(token) - 1] = '\0';

    char *end;
    uint32_t address = strtol(address_str, &end, base);

    return address;
}


// Returns true if post-indexed formatted address, false if pre-indexed
bool is_post_indexed_address(char *tokens[]) {
    // check that: [rX], or [rXX], are the first letters of the expression
    char *token = tokens[2];
    if (token[0] == '[' && token[1] == 'r') {
        return (token[3] == ']' && token[4] == ',' && token[5] != '\0') ||
            (token[4] == ']' && token[5] == ',' && token[6] != '\0');
    }
    return false;
}


// Gets the reg number for the base register in the form "[rX]" or "[rXX]"
uint16_t get_base_reg_indexed(char *base_reg_str) {
    char digit_buff[3];
    digit_buff[2] = '\0';

    if (strlen(base_reg_str) == 4) {
        digit_buff[0] = '0';
        digit_buff[1] = base_reg_str[2];
    } else {
        digit_buff[0] = base_reg_str[2];
        digit_buff[1] = base_reg_str[3];
    }

    return atoi(digit_buff);
}

// Helper function that gets the numeric offset (removes any leading # char)
int32_t get_numeric_offset(char *offset_str) {
    int base = offset_str[2] == 'x' || offset_str[3] == 'x' ? 16 : 10;
    
    int32_t offset;
    char *end;

    // if negative, string length is one less
    int len = offset_str[1] == '-' ? 2 : 1;

    char offset_num_str[strlen(offset_str) - len];
    memcpy(offset_num_str, &offset_str[len], strlen(offset_str));
    offset_num_str[strlen(offset_str) - len] = '\0';
    offset = strtol(offset_num_str, &end, base);
    
    return (len == 1 ? offset : -offset);
}


// Sets the bits if pre-indexed
void set_pre_indexed_address_bits(char *tokens[], uint32_t *bin_code) {
    char *reg_str = tokens[2];
    char *expr_str = tokens[3];

    uint16_t reg_val = get_base_reg_indexed(reg_str);
    int32_t offset = 0;

    // If expression is not empty
    if (expr_str != NULL) {
        offset = get_numeric_offset(expr_str);
    }

    if (offset < 0) {
        *bin_code &= ~(0x1 << 23); // if negative offset, U (bit 23) should be off
        offset = -offset;
    }

    *bin_code |= (uint32_t) offset;        // set offset at 11-0
    *bin_code |= reg_val << 16; // set Rn at 19-16
    *bin_code |= 1 << 24;       // set P bit at 24 (Pre indexed)
}


// Sets the bits if post-indexed
void set_post_indexed_address_bits(char *tokens[], uint32_t *bin_code) {
    char *base_reg_str = tokens[2];
    char *offset_str = tokens[3];
    
    // get offset
    uint32_t offset = get_numeric_offset(offset_str);

    // get Rn value
    uint16_t base_reg = get_base_reg_indexed(base_reg_str);
    
    *bin_code |= base_reg << 16;    // shift to bit 19-16
    *bin_code |= offset;            // set to bits 11-0
}


// Sets all other bits that rely on the addressing: I, P, U, Rn and Offset
// If a MOV instruction is to be called, it returns false
bool set_address_bits(char *tokens[], uint32_t *bin_code, uint32_t curr_instr_addr, 
    List *dumped_bytes_list, List *pending_offset_instr_addrs) {

    // Handle constant numeric address
    if (is_constant_address(tokens[2]) && (*bin_code & (1 << 20))) {
        uint32_t address_const = parse_constant_address(tokens[2]);
        if (address_const <= 0xFF) {
            return false;
        }

        *bin_code |= 1 << 24;       // set P bit at 24 (constant expr is pre-indexed by default)
        *bin_code |= 0xF << 16;     // set Rn to 0xF (not used)

        // add bytes to the list to be dumped later
        void *bytes = malloc(sizeof(uint32_t));
        if (!bytes) {
            perror("Error allocating memory for bytes in single_data_trans");
            exit(EXIT_FAILURE);
        }
        *((uint32_t *) bytes) = address_const;
        list_append(dumped_bytes_list, bytes);

        // mark the address of this instruction as pending for the offset calculation
        void *index = malloc(sizeof(uint32_t));
        if (!index) {
            perror("Error allocating memory for index in single_data_trans");
            exit(EXIT_FAILURE);
        }
        *((uint32_t *) index) = curr_instr_addr / 4;
        list_append(pending_offset_instr_addrs, index);
        
        return true;
    }

    // Handle pre/post indexing addresses
    if (is_post_indexed_address(tokens)) {
        set_post_indexed_address_bits(tokens, bin_code);
    } else {
        set_pre_indexed_address_bits(tokens, bin_code);
    }

    return true;
}
