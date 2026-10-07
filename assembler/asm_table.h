#ifndef ASM_TABLE_H
#define ASM_TABLE_H

#include <stdlib.h>

enum Commands
{
    HLT = 0,
    PUSH = 1,
    ADD = 2,
    SUB = 3,
    MULT = 4,
    DIV = 5,
    ABS = 6,
    SQRT = 7,
    OUT = 8,

    UNKNOWN = 1488,
};

const size_t N_COMMANDS = 9;

static const char *commands[N_COMMANDS] = {
    "HLT",
    "PUSH",
    "ADD",
    "SUB",
    "MULT",
    "DIV",
    "ABS",
    "SQRT",
    "OUT",
};

#endif // ASM_TABLE_H