CC      = gcc
CFLAGS  = -Wall -O3 -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -D_BSD_SOURCE -std=c99 -pedantic

EMULATOR_SOURCE = emulator/utilities.c emulator/cpu_state.c emulator/pipeline_executor.c emulator/multiply_instr.c emulator/branch_instr.c emulator/data_proc_instr.c emulator/single_data_transfer_instr.c emulator/barrel_shifter.c
ASSEMBLER_SOURCE = assembler/asm_utilities.c assembler/str_to_int_hash_tb.c

.SUFFIXES: .c .o

.PHONY: all clean

all: emulate assemble # uncomment for part 2

assemble: assemble.o
	$(CC) $(CFLAGS) assemble.c $(ASSEMBLER_SOURCE) -o $@

emulate: emulate.o
	$(CC) $(CFLAGS) emulate.c $(EMULATOR_SOURCE) -o $@

clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate
