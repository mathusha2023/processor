#ifndef FILES_H
#define FILES_H

#include <stddef.h>
#include "file_wrapper.h"

int get_file_text(struct FileWrapper *wrapper);
int get_file_size(const char *filename, size_t *size);
int read_text(const char *filename, char *buffer, size_t length, size_t *read_size);
int write_text(const char *filename, const char *line, const char *mode);
int write_string(const char *filename, struct String *str, const char *mode);
int write_strings(const char *filename, struct String *strings, size_t length, const char *mode);

#endif // FILES_H