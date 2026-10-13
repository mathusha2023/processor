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
int is_space_string(const char *str);
int is_num(const char *str);
int string_strcmp(struct String s1, const char *s2);
const char *string_chr(struct String *s, char c);
int make_strings_arr(struct FileWrapper *wrapper);

#endif // MY_STRINGS_H