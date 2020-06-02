#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "assembler/asm_utilities.h"
#include "assembler/hash_table.h"
#include "assembler/file_io.h"
#include "assembler/parser.h"



int read_red_num(char *reg){
    if (reg == 0){
        return 0;
    }
    return  atoi(strtok(reg, "r"));
}




static int int_ptr_cmp(const void *arg1, const void *arg2){
    char *i1 = (int *) arg1;
    char *i2 = (int *) arg2;
    return *i1 == *i2;
}

void test_ht(void){
    HashTable *ht = ht_create(int_ptr_cmp);
    int *k1 = malloc(sizeof(int));
    *k1 = 12;
    size_t size1 = sizeof(int);
    ht_set(ht, k1, strdup("asdasd"), size1);
   // printf("%s\n", ht_get(ht, k1, size1));
    ht_set(ht, k1, strdup("mikeanike"), size1);
   //  printf("%s\n", ht_get(ht, k1, size1));
    ht_del(ht, k1, size1);
    ht_free(ht);

}

int main(int argc, char **argv) {
    test_ht();
    assert (argc == 3);
    FILE *file = fopen(argv[1], "r");
    check_pointer(file, "File not found");

    int line_number = count_lines(file);
    char **lines = parse_to_array(file, line_number);
    fclose(file);

    encode_file_lines(lines, line_number);

    return 0;
}
