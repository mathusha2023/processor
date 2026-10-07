#ifndef MY_STRINGS_H
#define MY_STRINGS_H

#include <stddef.h>
#include <stdalign.h>
#include "file_wrapper.h"

struct String
{
    const char *p;
    size_t length;
};

int chrcount(const char *str, char c);
int lines_count(const char *str);
int make_strings_arr(struct FileWrapper *wrapper);

#endif // MY_STRINGS_H