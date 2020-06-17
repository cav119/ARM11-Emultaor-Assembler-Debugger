CC      = gcc
CFLAGS  = -Wall -O3 -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -D_BSD_SOURCE -std=c99 -pedantic

EMULATOR_SOURCE = emulator/utilities.c emulator/cpu_state.c emulator/pipeline_executor.c emulator/multiply_instr.c emulator/branch_instr.c emulator/data_proc_instr.c emulator/single_data_transfer_instr.c emulator/barrel_shifter.c extension/command_parser.c extension/extension_utilities.c assembler/hash_table.c
ASSEMBLER_SOURCE = assembler/asm_utilities.c assembler/hash_table.c assembler/parser.c assembler/asm_single_data_transfer_instr.c assembler/asm_data_proc_instr.c assembler/asm_multiply_instr.c assembler/asm_branch_instr.c assembler/list.c assembler/array_list.c

.SUFFIXES: .c .o

.PHONY: all clean

all: emulate assemble 

assemble: assemble.o
	$(CC) $(CFLAGS) assemble.c $(ASSEMBLER_SOURCE) -o $@

emulate: emulate.o
	$(CC) $(CFLAGS) emulate.c $(EMULATOR_SOURCE) -o $@

clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate
