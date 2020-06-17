CC = gcc
CFLAGS = -Wall -pedantic -std=c99 -g
LDFLAGS = -lcurses

SOURCE_FILES = gui.c str_list.c command_parser.c ../src/emulator/utilities.c ../src/emulator/cpu_state.c ../src/emulator/pipeline_executor.c ../src/emulator/multiply_instr.c ../src/emulator/branch_instr.c ../src/emulator/data_proc_instr.c ../src/emulator/single_data_transfer_instr.c ../src/emulator/barrel_shifter.c ../src/assembler/hash_table.c

.PHONY = clean

debugger:
	$(CC) $(CFLAGS) main.c $(SOURCE_FILES) -o $@ $(LDFLAGS)

clean:
	rm debugger
	rm -f *.o
