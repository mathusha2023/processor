#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "asm_table.h"
#include "file_wrapper.h"

enum ProcessStringError
{
    PSE_OK,
    PSE_EMPTY_STRING,
    PSE_COPY_ERROR,
    PSE_INCORRECT_INPUT,
    PSE_UNKNOWN_COMMAND,
    PSE_INVALID_ARGS,
};

struct IR
{
    enum Commands cmd;
    size_t cmd_args_count;
    int arg1;
    int arg2;
};

int asm_data(struct FileWrapper *file, const char *output_file_name, const char *output_debug_file_name);

#endif // ASSEMBLER_H
