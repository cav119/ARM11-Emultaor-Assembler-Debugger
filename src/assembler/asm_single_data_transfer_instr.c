#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>



void print_bits(uint32_t x);

// should make a very useful function:

// uint32_t parse_number(const char *str, int start, int end);

void set_ldr_str_bit(const char *token, uint32_t *bin_code);
void set_transfer_reg_bits(const char *token, uint32_t *bin_code);
void set_address_bits(const char *token, uint32_t *bin_code);



int encode_sdt_instr_to_binary(int argc, char **argv) {
    char cond = 0xE; // default cond is set to 1110 in 31-28
    uint32_t bin_code = cond << 28 | 1 << 26; // set 01 in 27-26
    // char instr[] = "ldr r12, =0x555555";
    char instr[] = "ldr r9, [r11],#0x123";
    printf("Instruction is: %s\n", instr);
    print_bits(bin_code);

    // General syntax: <ldr/str> Rd, <address_expression>
    // the <address_expression> has no spaces in it

    const char delim[2] = " ";
    char *token;

    token = strtok(instr, delim);
    int i = 0;
    while (token != NULL) {
        printf("%s\n", token);
        switch (i) {
            case 0: // Handle 1st token: <ldr/str>
                set_ldr_str_bit(token, &bin_code);
                break;
            case 1: // Handle 2nd token: Rd
                set_transfer_reg_bits(token, &bin_code);
                break;
            case 2: // Handle 3rd token: <address_expression>
                set_address_bits(token, &bin_code);
                break;
            default:
                printf("Invalid instruction\n");
                exit(EXIT_FAILURE);
        }

        // get next token in string
        token = strtok(NULL, delim);
        i++;
    }

    print_bits(bin_code);
    return EXIT_SUCCESS;
}





// Sets L bit depending if token is ldr or str in the binary code
void set_ldr_str_bit(const char *token, uint32_t *bin_code) {
    if (strcmp(token, "ldr") == 0) {
        *bin_code |= 1 << 20; // set bit 20
    }
}




// Sets the transfer register (Rd) bits (15-12) in the binary code
void set_transfer_reg_bits(const char *token, uint32_t *bin_code) {
    // rX,
    // If 3rd char is a comma, then the register is 1 digit (0-9)
    // otherwise, we know it's 2 digits (assuming well-formed syntax)
    char reg_num;
    if (token[2] == ',') {
        reg_num = atoi(&token[1]);
    } else {
        // parse token[1] and token[2] into a single number
        // this method works but probably not the best
        char digits[3];
        memcpy(digits, &token[1], 2);
        digits[2] = '\0';
        reg_num = atoi(digits);
    }

    //assert reg_num < total number of regs


    // set bits 15-12 for Rd
    printf("Rd = %d\n", reg_num);
    *bin_code |= reg_num << 12;
}


////////// ADDRESSING ///////////


// Returns true if address is a numeric constant, false if indexed
bool is_constant_address(const char *token) {
    if (token[0] == '=') {
        return true;
    }

    return false;
}



// Returns true if pre-indexed address, false if post-indexed
bool is_pre_indexed_address(const char *token) {

    

}



// Sets the bits if pre-indexed
void set_pre_indexed_address_bits(const char *token, uint32_t *bin_code) {

    // check all three modes

}



// Sets the bits if post-indexed
void set_post_indexed_address_bits(const char *token, uint32_t *bin_code) {
    // token = "[Rn],<#bytes>"
    const char *delim = ",";
    char *base_reg_str = strtok(token, delim); // [rXX]
    char *offset_str = strtok(NULL, delim); // #0xBBBBBB
    
    // get offset bytes (need to make a substring function)
    int base = offset_str[2] == 'x' ? 16 : 10;
    char offset[strlen(offset_str) - 1];
    memcpy(offset, &offset_str[1], strlen(offset_str));
    offset[strlen(offset_str) - 1] = '\0';
    char *ptr;
    long t = strtol(offset, ptr, base);

    // get Rn
    char reg[4];
    memcpy(reg, &base_reg_str[1], 4);
    reg[4] = '\0';
    
    int regn = atoi(reg);

    printf("offset = %ld base-reg = %d\n", t, regn);
}

int read_reg_num(char *reg){
    if (reg == 0){
        return 0;
    }
    return  atoi(strtok(reg, "r"));
}



// Sets all other bits that rely on the addressing: I, P, U, Rn and Offset
void set_address_bits(const char *token, uint32_t *bin_code) {
    set_post_indexed_address_bits(token, bin_code);

    if (is_constant_address(token)) {
        // if (parse_constant_address(token) <= 0xFF) {
        //     // call mov instruction (it deals with ALL the bits)
        //     return;
        // }

        // deal with general case
        return;
    }

    if (is_pre_indexed_address(token)) {
        set_pre_indexed_address_bits(token, bin_code);
    } else {
        set_post_indexed_address_bits(token, bin_code);
    }
}



void print_bits(uint32_t x) {
    int i;
    uint32_t mask = 1 << 31;

    for(i=0; i<32; ++i) {
        if((x & mask) == 0){
            printf("0");
        }
        else {
            printf("1");
        }
        x = x << 1;
    }
    printf("\n");
}
