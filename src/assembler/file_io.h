#ifndef FILE_IO_H
#define FILE_IO_H
#include <stdint.h>
#include <stdio.h>

#define MAX_LINE_LENGTH (512)

// Counts the number of lines in the given file
int count_lines(FILE *file);


// Parses the lines of a file to an array of strings
char **parse_to_array(FILE *file, int lines);
#endif

