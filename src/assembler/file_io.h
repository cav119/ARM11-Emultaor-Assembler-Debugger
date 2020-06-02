#ifndef FILE_IO_H
#define FILE_IO_H
#include <stdint.h>
#include <stdio.h>

#define MAX_LINE_LENGTH (512)

// Counts the number of lines in the given file
int count_lines(FILE *file);

// transforms a legitimate line to 5 words
char **line_to_words(char array[]);

// Parses the lines of a file to an array of strings
char **parse_to_array(FILE *file, int lines);
#endif

