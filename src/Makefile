CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -D_BSD_SOURCE -std=c99 -pedantic

EMULATOR_SOURCE = emulator/utilities.c emulator/cpu_state.c emulator/pipeline_executor.c emulator/multiply_instr.c emulator/branch_instr.c


.SUFFIXES: .c .o

.PHONY: all clean

all: emulate #assemble # uncomment for part 2

# uncomment this for part 2
# assemble: assemble.o
# 	$(CC) $(CFLAGS) $^ -o $@

emulate: emulate.o
	$(CC) $(CFLAGS) emulate.c $(EMULATOR_SOURCE) -o $@

clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate
