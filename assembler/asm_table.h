#ifndef ASM_TABLE_H
#define ASM_TABLE_H

#include <stddef.h>

enum Commands
{
    HLT = 0,
    PUSH_NUMBER = 1,
    ADD = 2,
    SUB = 3,
    MUL = 4,
    DIV = 5,
    ABS = 6,
    SQRT = 7,
    OUT = 8,
    POP = 9,
    PUSH_REGISTER = 10,

    UNKNOWN = 1488,
};

enum ArgTypes
{
    ATYPE_VOID,
    ATYPE_INT,
    ATYPE_REGISTER,
};

struct Command
{
    enum Commands cmd_num;
    const char *cmd_str;
    const size_t cmd_args_count;
    const enum ArgTypes arg1_type;
    const enum ArgTypes arg2_type;
};

const size_t N_COMMANDS = 11;

const static struct Command commands[N_COMMANDS] = {
    {
        .cmd_num = HLT,
        .cmd_str = "HLT",
        .cmd_args_count = 0,
    },

    {
        .cmd_num = PUSH_NUMBER,
        .cmd_str = "PUSH",
        .cmd_args_count = 1,
        .arg1_type = ATYPE_INT,
    },

    {
        .cmd_num = ADD,
        .cmd_str = "ADD",
        .cmd_args_count = 0,
    },

    {
        .cmd_num = SUB,
        .cmd_str = "SUB",
        .cmd_args_count = 0,
    },

    {
        .cmd_num = MUL,
        .cmd_str = "MUL",
        .cmd_args_count = 0,
    },

    {
        .cmd_num = DIV,
        .cmd_str = "DIV",
        .cmd_args_count = 0,
    },

    {
        .cmd_num = ABS,
        .cmd_str = "ABS",
        .cmd_args_count = 0,
    },

    {
        .cmd_num = SQRT,
        .cmd_str = "SQRT",
        .cmd_args_count = 0,
    },

    {
        .cmd_num = OUT,
        .cmd_str = "OUT",
        .cmd_args_count = 0,
    },

    {
        .cmd_num = POP,
        .cmd_str = "POP",
        .cmd_args_count = 1,
        .arg1_type = ATYPE_REGISTER,
    },

    {
        .cmd_num = PUSH_REGISTER,
        .cmd_str = "PUSH",
        .cmd_args_count = 1,
        .arg1_type = ATYPE_REGISTER,
    },
};

#endif // ASM_TABLE_H