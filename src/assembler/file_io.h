#ifndef FILE_IO_H
#define FILE_IO_H
#include <stdint.h>
#include <stdio.h>

// Parses the assembly input file line by line and returns an array of strings
char **parse_lines(FILE *input_file);

// Writes binary code to an (open) file in little endian
void write_bytes(FILE *output_file, uint32_t bin_code);


#endif
