#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "assembler/asm_utilities.h"
#include "assembler/hash_table.h"

int count_lines(FILE *file){
    int count = 0;
    char prev = 'n';
    for (char c = getc(file); c != EOF; c = getc(file)){
        if (c == '\n' && prev != '\n'){
            count++;
        }
        prev = c;
    }
    if (prev != '\n'){
        count++;
    }
    rewind(file);
    return count;
}

char **parse_to_array(FILE *file, int lines){
    char **array = (char **) malloc(lines * sizeof(char *));

    for (int i = 0; i < lines ; i++) {
        array[i] = (char *) malloc(512 * sizeof(char));
        fgets(array[i], 512, file);
    }
    return array;
}

char **line_to_words(char array[]){
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


int read_red_num(char *reg){
    if (reg == 0){
        return 0;
    }
    return  atoi(strtok(reg, "r"));
}


int str_cmp(const void *arg1, const void *arg2){
    char *str1 = (char *) arg1;
    char *str2 = (char *) arg2;
    return strcmp(str1, str2) == 0;

}

int int_ptr_cmp(const void *arg1, const void *arg2){
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
    printf("%s\n", ht_get(ht, k1, size1));
    ht_set(ht, k1, strdup("mikeanike"), size1);
    printf("%s\n", ht_get(ht, k1, size1));
    ht_del(ht, k1, size1);
    ht_free(ht);


}

int main(int argc, char **argv) {
    test_ht();
    assert (argc == 3);
    FILE *file = fopen(argv[1], "r");
    check_pointer(file, "File not found");
    int lines = count_lines(file);
    char **array = parse_to_array(file, lines);
    fclose(file);

    char **array_of_words[lines];

    for(int i = 0; i < lines; i++){
        array_of_words[i] = line_to_words(array[i]);
    }

    for (int i = 0; i < lines; i++) {
        for (int j = 0; j < 5; j++) {
            printf("j = %d , %s\n",j, array_of_words[i][j]);
        }
        printf("\n");
    }

    return 0;
}
