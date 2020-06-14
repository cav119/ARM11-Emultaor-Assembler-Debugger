#ifndef EXTENSION_COMMAND_PARSER_H
#define EXTENSION_COMMAND_PARSER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "commands.h"
#include "../emulator/cpu_state.h"
#include "break_list.h"

#define MAX_COMMAND_LEN (100)

// Gets input and executes a command, returns whether the user
// hit the exit command that halts the "debugger"
bool get_input_and_execute(CpuState *cpu_state, List *list);

#endif
