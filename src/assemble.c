#include <stdio.h>
#include <assert.h>

#include "assembler/asm_utilities.h"
#include "assembler/parser.h"


int main(int argc, char **argv) {
    assert (argc == 3);

    FILE *inp_file = fopen(argv[1], "r");
    check_pointer(inp_file, "File not found");

    FILE *out_file = fopen(argv[2], "wb");
    check_pointer(out_file, "File not found");

    one_pass_assemble(inp_file, out_file);
    fclose(inp_file);
    fclose(out_file);

    return EXIT_SUCCESS;
}
