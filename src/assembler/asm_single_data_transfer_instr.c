#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "asm_single_data_transfer_instr.h"


// Internal helper functions 
static void set_ldr_str_bit(const char *token, uint32_t *bin_code);
static void set_transfer_reg_bits(const char *token, uint32_t *bin_code);
static bool set_address_bits(const char *token, uint32_t *bin_code, uint32_t curr_instr, 
    List *dumped_bytes_list, List *pending_offset_instr_addrs);


uint32_t encode_sdt_instr_to_binary(char *tokens[], uint32_t curr_instr, List *dumped_bytes_list, 
    List *pending_offset_instr_addrs) {

    char cond = 0xE; // default condition code is set to 1110 in 31-28
    uint32_t bin_code = cond << 28 | 1 << 26; // set 01 in 27-26

    set_ldr_str_bit(tokens[0], &bin_code);
    set_transfer_reg_bits(tokens[1], &bin_code);
    bool not_mov = set_address_bits(tokens[2], &bin_code, curr_instr, dumped_bytes_list, pending_offset_instr_addrs);

    if (!not_mov) {
        // call to encode_data_proc_inst instead (treated as a mov instr)
        // return encode_data_proc_instr(tokens);
    }

    return bin_code;
}


// Sets L bit depending if token is ldr or str in the binary code
void set_ldr_str_bit(const char *token, uint32_t *bin_code) {
    if (strcmp(token, "ldr") == 0) {
        *bin_code |= 1 << 20; // set bit 20
    }
}

// maybe put in utils
int read_reg_num(char *reg){
    if (reg == 0){
        return 0;
    }
    
    return atoi(strtok(reg, "r"));
}

// Sets the transfer register (Rd) bits (15-12) in the binary code
void set_transfer_reg_bits(const char *token, uint32_t *bin_code) {
    char reg_num = read_reg_num(token);
    *bin_code |= reg_num << 12; // set bits 15-12 for Rd

    printf("Rd = %d\n", reg_num);
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
bool is_post_indexed_address(char *token) {
    // check that: [rX], or [rXX], are the first letters of the expression
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
uint32_t get_numeric_offset(char *offset_str) {
    int base = offset_str[2] == 'x' ? 16 : 10;
    char offset_num_str[strlen(offset_str) - 1];
    memcpy(offset_num_str, &offset_str[1], strlen(offset_str));
    offset_num_str[strlen(offset_str) - 1] = '\0';

    char *end;
    uint32_t offset = strtol(offset_num_str, &end, base);
    return offset;
}


// Sets the bits if pre-indexed
void set_pre_indexed_address_bits(char *token, uint32_t *bin_code) {
    char *reg_str = strtok(token, ","); 
    char *expr_str = strtok(NULL, ","); 

    uint16_t reg_val = get_base_reg_indexed(reg_str);
    uint32_t offset = 0;

    // If expression is empty
    if (expr_str != NULL) {
        printf("empty expression, set offset to 0\n");
        offset = get_numeric_offset(expr_str);
    }

    printf("reg value = %d\n", reg_val);
    printf("OFFSET value = 0x%x\n", offset);

    *bin_code |= offset;        // set offset at 11-0
    *bin_code |= reg_val << 16; // set Rn at 19-16
    *bin_code |= 1 << 24;       // set P bit at 24 (Pre indexed)
}


// Sets the bits if post-indexed
void set_post_indexed_address_bits(char *token, uint32_t *bin_code) {
    char *base_reg_str = strtok(token, ","); 
    char *offset_str = strtok(NULL, ","); 
    
    // get offset
    uint32_t offset = get_numeric_offset(offset_str);

    // get Rn value
    uint16_t base_reg = get_base_reg_indexed(base_reg_str);
    
    *bin_code |= base_reg << 16;    // shift to bit 19-16
    *bin_code |= offset;            // set to bits 11-0
}


// Sets all other bits that rely on the addressing: I, P, U, Rn and Offset
// If a MOV instruction is to be called, it returns false
bool set_address_bits(const char *token, uint32_t *bin_code, uint32_t curr_instr_addr, 
    List *dumped_bytes_list, List *pending_offset_instr_addrs) {

    char tok_copy[strlen(token) + 1];
    strcpy(tok_copy, token);

    // Handle constant numeric address
    if (is_constant_address(tok_copy) && (*bin_code & (1 << 20))) {
        uint32_t address_const = parse_constant_address(tok_copy);
        // printf("constant address: %d\n", address_expr);
        if (address_const <= 0xFF) {
            return false;
        }

        // deal with general case
        // add to pending_offset_instr_addrs the address of this instr (curr_instr_addr)
        // leave the offset uncalculated (these bits are set later)

        // add bytes to the list
        void *bytes = malloc(sizeof(uint32_t));
        *((uint32_t *) bytes) = address_const;
        list_append(dumped_bytes_list, bytes);

        // mark the address of this instruction as pending for the offset calculation
        void *index = malloc(sizeof(uint32_t));
        *((uint32_t *) index) = curr_instr_addr;
        list_append(pending_offset_instr_addrs, index);

        /* Once all the instructions have been encoded and placed in an instruction table
           ready to be written to the file, do the following (in the parser.c file):

            // uint32_t instruction_array[] holds all the instruction bytes (4 bytes per element) after being encoded in the main parse loop

           // IN THEORY, THERE SHOULD BE THE SAME NUMBER OF ELEMENTS IN BOTH LISTS,
           // EACH CORRESPONDING TO THE ADDR OF THE INSTR IN WHICH TO CALCULATE THE OFFSET
           // loop through both lists
           int last_instr_idx = total number of instructions - 1;
           ListNode *curr_bytes_node = dumped_bytes_list->head;
           ListNode *curr_addr_node = pending_offset_instr_addrs->head;
           int data_index = 1;
           while (curr_bytes_node != NULL && curr_addr_node != NULL) {
               uint32_t instr_addr = *((int *)pending_offset_instr_addrs->elem);    // this is the index to the instruction array of a pending instruction
               instruction_array[instr_addr] |= (last_instr_idx + data_index - instr_addr) // set offset bits as needed
               instruction_array[last_instr_idx + data_index] = *((uint32_t *)curr_bytes_node->elem); // write the bytes at the end of array
               data_index++;
               curr_node = curr_node->next;
               curr_addr_node = curr_addr_node->next;
           }

           // write the instruction array to the file

           // free both lists and other data structurs
        */

        return true;
    }

    // Handle pre/post indexing addresses
    if (is_post_indexed_address(tok_copy)) {
        printf("IS POST INDEXED\n");
        set_post_indexed_address_bits(tok_copy, bin_code);
    } else {
        printf("IS PRE INDEXED\n");
        set_pre_indexed_address_bits(tok_copy, bin_code);
    }

    return true;
}
