#ifndef EXTENSION_COMMAND_PARSER_H
#define EXTENSION_COMMAND_PARSER_H

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

#include "commands.h"
#include "../src/emulator/cpu_state.h"
#include "../src/assembler/hash_table.h"
#include "gui.h"


#define MAX_COMMAND_LEN (100)

// Gets input and executes a command, returns whether the user
// hit the exit command that halts the "debugger"
bool get_input_and_execute(CpuState *cpu_state, bool *is_stepping, HashTable *ht, MainWin *win);

#endif
