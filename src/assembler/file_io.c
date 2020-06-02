#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "file_io.h"
#include "asm_utilities.h"


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
        array[i] = (char *) malloc(MAX_LINE_LENGTH * sizeof(char));
        fgets(array[i], MAX_LINE_LENGTH, file);
    }
    return array;
}

char **line_to_words(char array[]){
    char **words = calloc(5, MAX_LINE_LENGTH);
    char *arr = strtok(array, " ,:\n");
    int i = 0;
    while (arr != NULL){
        words[i] = arr;
        arr = strtok(NULL, " ,:\n");
        i++;
    }
    return words;
}
