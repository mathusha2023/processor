#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "asm_table.h"

int asm_data(struct FileWrapper *file, const char *output_file_name);
enum Commands get_command(char *command);

#endif // ASSEMBLER_H
