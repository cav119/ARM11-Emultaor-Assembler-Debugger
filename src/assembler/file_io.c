#include <assert.h>
#include "file_io.h"
#include "asm_utilities.h"

#define MAX_LINE_LENGTH 512

// Internal helper function to calculate the number of non-empty lines in a file
static size_t count_file_lines(FILE *input_file) {
    size_t count = 0;
    char prev = 'n';
    for (char c = getc(input_file); c != EOF; c = getc(input_file)) {
        if (c == '\n' && prev != '\n') {
            count++;
        }
        prev = c;
    }
    if (prev != '\n') {
        count++;
    }
    rewind(input_file);

    return count;
}

// Probably gonna cause memory leaks, it is the responsability of the caller
// to free the rows and the entire array at the end of the program
char **parse_lines(FILE *input_file) {
    size_t num_lines = count_file_lines(input_file);
    char **array = (char **) malloc(num_lines * sizeof(char *));

    for (int i = 0; i < num_lines ; i++) {
        array[i] = (char *) malloc(MAX_LINE_LENGTH * sizeof(char));
        fgets(array[i], MAX_LINE_LENGTH, num_lines);
    }

    return array;
}


void write_bytes(FILE *ouput_file, uint32_t bin_code) {
    char bytes[] = {
        process_mask(bin_code, 0, 7),
        process_mask(bin_code, 8, 15),
        process_mask(bin_code, 16, 23),
        process_mask(bin_code, 24, 31),
    };

    // File mode should be ab+
    size_t written = fwrite(bytes, 1, 4, ouput_file);
    assert(written == 4);
}
