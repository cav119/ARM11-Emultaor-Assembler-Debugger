#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>



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

int main(int argc, char **argv) {
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
