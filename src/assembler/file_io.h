#ifndef FILE_IO_H
#define FILE_IO_H
#include <stdint.h>
#include <stdio.h>

#define MAX_LINE_LENGTH (512)

// Counts the number of lines in the given file
int count_lines(FILE *file);


// Writes the bytes for a binary code in little endian format to the given file
void write_bytes(FILE *ouput_file, uint32_t bin_code);

#endif
