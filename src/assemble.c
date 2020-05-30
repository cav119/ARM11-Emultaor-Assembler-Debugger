#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "assembler/str_to_int_hash_tb.h"


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

int32_t setBit(uint32_t n, int pos){
    n |= (1 << pos);
    return n;
}


int read_red_num(char *reg){
    if (reg == 0){
        return 0;
    }
    return  atoi(strtok(reg, "r"));
}

uint32_t encode_multiply(char *instr[]){
    uint32_t n = (111 << 29);
    n = setBit(n, 4);
    n = setBit(n, 7);
    n |= (read_red_num(instr[1]) << 16);
    n|= read_red_num(instr[2]);
    n|= (read_red_num(instr[3]) << 8);

    if (strcasecmp(instr[0], "mull") != 0){
        n = setBit(n, 21);
        n|= (read_red_num(instr[4]) << 12);
    }
return n;

}

int str_cmp(const void *str1, const void *str2){
   char *string1 = (char *) str1;
   char *string2 = (char *) str2;
   return strcmp(string1, string2) == 0; 
}

int int_cmp(const void *p1, const void *p2){
    int *int1 = (int *) p1;
    int *int2 = (int *) p2;
    return *int1 == *int2;
}

int ip(int *key, Dict d){
    int *intptr = dict_search(d, key);
    return *intptr;
}


void dict_test(void){
   Dict d = dict_create(int_cmp);
   int i1 = 3;
   int i2 = 19;
   int foo = 1;
   dict_insert(d, &foo, &i1);
    printf("%d\n", ip(&foo, d));
    dict_insert(d, &foo, &i2);
   printf("%d\n", ip(&foo, d));
   dict_delete(d, &foo);
   dict_destroy(d); 

}

int main(int argc, char **argv) {
    dict_test();
    assert (argc == 3);
    FILE *file = fopen(argv[1], "r");

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
