#ifndef EXTENSION_COMMANDS_H
#define EXTENSION_COMMANDS_H

#include <stdbool.h>
#include <stdint.h>

#define PROMPT_TXT ("> ")
#define PROMPT_SIZE 2
#define STRING(str) #str

#define RUN_CMD_L STRING("run")
#define RUN_CMD_S STRING("r")
#define NEXT_CMD_L STRING("next")
#define NEXT_CMD_S STRING("n")
#define BREAK_CMD_S STRING("b")
#define BREAK_CMD_L STRING("break")
#define PRINT_CMD_S STRING("p")
#define PRINT_CMD_L STRING("print")
#define EXIT_CMD_L STRING("exit")
#define EXIT_CMD_S STRING("ex")

// print command examples:
// print HEX R15
// print BIN R16
// print DEC M16
// p DEC R16

typedef enum { RUN_CMD, NEXT_CMD, BREAK_CMD, EXIT_CMD, PRINT_CMD} CommandsEnum;

// Print formats for the print commands
typedef enum { HEX = 0, BIN = 1, DEC = 2} PrintFormat;

typedef struct print_cmd {
   PrintFormat format;
   bool is_register_print;
   uint32_t print_target; 
} PrintCommand;

typedef struct{
    CommandsEnum type;
    union {
        PrintCommand *print;
     } command;
} ExecutableCommand;

#endif
