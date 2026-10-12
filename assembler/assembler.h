#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "asm_table.h"
#include "file_wrapper/file_wrapper.h"
#include "file_wrapper/my_string.h"
#include "config.h"

enum ProcessStringError
{
    PSE_OK,
    PSE_EMPTY_STRING,
    PSE_COPY_ERROR,
    PSE_INCORRECT_INPUT,
    PSE_UNKNOWN_COMMAND,
    PSE_INVALID_ARGS,
    PSE_IS_LABEL,
};

struct IR
{
    enum Commands cmd;
    size_t cmd_args_count;
    int arg1;
    int arg2;
};

struct Label
{
    struct String str;
    size_t position;
};

struct ASM
{
    struct FileWrapper *file;
    long long code[MAX_CODE_LENGTH];
    size_t code_index;
    struct Label labels[MAX_LABELS_COUNT];
    size_t labels_count;
    int labels_ready;
};

const size_t REGISTERS_COUNT = 8;
static const char *REGISTERS[REGISTERS_COUNT] = {
    "0X",
    "AX",
    "BX",
    "CX",
    "DX",
    "EX",
    "FX",
    "GX",
};

int asm_data(struct FileWrapper *file, const char *output_file_name, const char *output_debug_file_name);

#endif // ASSEMBLER_H
