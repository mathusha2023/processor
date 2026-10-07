#ifndef FILE_WRAPPER_H
#define FILE_WRAPPER_H

#include <stddef.h>

struct FileWrapper
{
    const char *filename;

    char *buffer;
    size_t file_size;
    size_t file_n_symb; // на линукс и макос равен file_size, однако на винде может быть меньше из за формата \r\n

    struct String *strings;
    size_t strings_count;

    int error;

    void (*dispose)(struct FileWrapper *wrapper);
};

struct FileWrapper init_file_wrapper(const char *filename);

#endif // FILE_WRAPPER_H