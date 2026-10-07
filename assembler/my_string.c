#include "my_string.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "log.h"
#include "file_wrapper.h"

int chrcount(const char *str, char c)
{
    assert(str);

    int count = 0;
    for (size_t i = 0; str[i]; i++)
    {
        if (str[i] == c)
            count++;
    }
    return count;
}

int lines_count(const char *str)
{
    assert(str);

    return chrcount(str, '\n') + 1;
}

// устанавливает поля wrapper->strings и wrapper->strings_count
int make_strings_arr(struct FileWrapper *wrapper)
{
    assert(wrapper);
    assert(wrapper->buffer);
    assert(!wrapper->strings);

    int lines_c = lines_count(wrapper->buffer);
    wrapper->strings_count = 0; // отдельный счетчик для обработки случая, когда после последнего \n идет \0

    struct String *arr = (struct String *)calloc((size_t)lines_c, sizeof(struct String));
    if (!arr)
    {
        int error = errno;
        log("Cant allocate memory: error %d((", error);
        wrapper->error = error;
        return error;
    }

    const char *start_p = wrapper->buffer;
    size_t i = 0, arr_index = 0;

    for (i = 0, arr_index = 0; wrapper->buffer[i]; i++)
    {
        if (wrapper->buffer[i] == '\n')
        {
            struct String string = {.p = start_p, .length = (size_t)(wrapper->buffer + i + 1 - start_p)};
            arr[arr_index++] = string;
            start_p = wrapper->buffer + i + 1;
            wrapper->strings_count++;
        }
    }

    // только если после последнего перевода строки все еще есть строка добавляем ее
    if (*start_p)
    {
        wrapper->strings_count++;

        // без добавления 1, чтобы строка не заканчивалась на \0
        struct String string = {.p = start_p, .length = (size_t)(wrapper->buffer + i - start_p)};
        arr[arr_index] = string;
    }
    wrapper->strings = arr;

    return 0;
}