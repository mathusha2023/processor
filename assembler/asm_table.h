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
    JMP = 11,
    JA = 12,
    JB = 13,
    JAE = 14,
    JBE = 15,
    JE = 16,
    JNE = 17,
    CALL = 18,
    RET = 19,
    IN = 20,
    OUTC = 21,
    DORA = 22,
    RND = 23,

    UNKNOWN = 1488,
};

enum ArgTypes
{
    ATYPE_VOID,
    ATYPE_INT,
    ATYPE_REGISTER,
    ATYPE_LABEL,
};

struct Command
{
    enum Commands cmd_num;
    const char *cmd_str;
    const size_t cmd_args_count;
    const enum ArgTypes arg1_type;
    const enum ArgTypes arg2_type;
};

const size_t N_COMMANDS = 24;

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

    {
        .cmd_num = JMP,
        .cmd_str = "JMP",
        .cmd_args_count = 1,
        .arg1_type = ATYPE_LABEL,
    },

    {
        .cmd_num = JA,
        .cmd_str = "JA",
        .cmd_args_count = 1,
        .arg1_type = ATYPE_LABEL,
    },

    {
        .cmd_num = JB,
        .cmd_str = "JB",
        .cmd_args_count = 1,
        .arg1_type = ATYPE_LABEL,
    },

    {
        .cmd_num = JAE,
        .cmd_str = "JAE",
        .cmd_args_count = 1,
        .arg1_type = ATYPE_LABEL,
    },

    {
        .cmd_num = JBE,
        .cmd_str = "JBE",
        .cmd_args_count = 1,
        .arg1_type = ATYPE_LABEL,
    },

    {
        .cmd_num = JE,
        .cmd_str = "JE",
        .cmd_args_count = 1,
        .arg1_type = ATYPE_LABEL,
    },

    {
        .cmd_num = JNE,
        .cmd_str = "JNE",
        .cmd_args_count = 1,
        .arg1_type = ATYPE_LABEL,
    },

    {
        .cmd_num = CALL,
        .cmd_str = "CALL",
        .cmd_args_count = 1,
        .arg1_type = ATYPE_LABEL,
    },

    {
        .cmd_num = RET,
        .cmd_str = "RET",
        .cmd_args_count = 0,
    },

    {
        .cmd_num = IN,
        .cmd_str = "IN",
        .cmd_args_count = 0,
    },

    {
        .cmd_num = OUTC,
        .cmd_str = "OUTC",
        .cmd_args_count = 0,
    },

    {
        .cmd_num = DORA,
        .cmd_str = "DORA",
        .cmd_args_count = 0,
    },

    {
        .cmd_num = RND,
        .cmd_str = "RND",
        .cmd_args_count = 0,
    },

};

#endif // ASM_TABLE_H