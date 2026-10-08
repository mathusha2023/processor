#ifndef ASM_TABLE_H
#define ASM_TABLE_H

#include <stdlib.h>
#include "assembler.h"

enum Commands
{
    HLT = 0,
    PUSH_NUMBER = 1,
    ADD = 2,
    SUB = 3,
    MULT = 4,
    DIV = 5,
    ABS = 6,
    SQRT = 7,
    OUT = 8,

    UNKNOWN = 1488,
};

struct Command
{
    enum Commands cmd_num;
    const char *cmd_str;
    const size_t cmd_args_count;
};

const size_t N_COMMANDS = 9;

const static struct Command commands[N_COMMANDS] = {
    {.cmd_num = HLT,
     .cmd_str = "HLT",
     .cmd_args_count = 0},

    {.cmd_num = PUSH_NUMBER,
     .cmd_str = "PUSH",
     .cmd_args_count = 1},

    {.cmd_num = ADD,
     .cmd_str = "ADD",
     .cmd_args_count = 0},

    {.cmd_num = SUB,
     .cmd_str = "SUB",
     .cmd_args_count = 0},

    {.cmd_num = MULT,
     .cmd_str = "MULT",
     .cmd_args_count = 0},

    {.cmd_num = DIV,
     .cmd_str = "DIV",
     .cmd_args_count = 0},

    {.cmd_num = ABS,
     .cmd_str = "ABS",
     .cmd_args_count = 0},

    {.cmd_num = SQRT,
     .cmd_str = "SQRT",
     .cmd_args_count = 0},

    {.cmd_num = OUT,
     .cmd_str = "OUT",
     .cmd_args_count = 0},
};

#endif // ASM_TABLE_H