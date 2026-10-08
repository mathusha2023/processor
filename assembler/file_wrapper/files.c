#include "files.h"
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "config.h"
#include "log.h"
#include "my_string.h"
#include "file_wrapper.h"

// устанавливает поля wrapper->file_size, wrapper->buffer, wrapper->file_n_symb
int get_file_text(struct FileWrapper *wrapper)
{
    assert(wrapper);
    assert(wrapper->filename);

    size_t file_size = 0;
    int error = 0;

    error = get_file_size(wrapper->filename, &file_size);
    if (error)
    {
        wrapper->error = error;
        log("Error getting file size: %d", error);
        return error;
    }
    wrapper->file_size = file_size;

    char *buffer = (char *)calloc(file_size + 1, sizeof(char));
    if (!buffer)
    {
        error = errno;
        wrapper->error = error;
        log("Can not alloc memory (%d)", error);
        return error;
    }

    size_t read_size = 0;
    error = read_text(wrapper->filename, buffer, file_size + 1, &read_size);
    if (error)
    {
        wrapper->error = error;
        log("Error while reading file: %d", error);
        free_ptr(buffer);
        return error;
    }
    wrapper->buffer = buffer;
    wrapper->file_n_symb = read_size;

    return 0;
}

int get_file_size(const char *filename, size_t *size)
{
    assert(filename);
    assert(size);

    struct stat stat_struct = {};

    if (stat(filename, &stat_struct) == -1)
    {
        int error = errno;
        log("Cannot get stat from file %s (err_code = %d)", filename, error);
        return error;
    };

    *size = (size_t)stat_struct.st_size;
    return 0;
}

int read_text(const char *filename, char *buffer, size_t length, size_t *read_size)
{
    assert(filename);
    assert(buffer);

    FILE *file = fopen(filename, "r");
    if (!file)
    {
        int error = errno;
        log("Cannot open file %s (error %d)", filename, error);
        return error;
    }

    *read_size = fread((void *)buffer, sizeof(buffer[1488]), length - 1, file);
    if (ferror(file))
    {
        int error = errno;
        log("Error while reading file %s (error %d)", filename, error);
        fclose(file);
        return error;
    }
    buffer[*read_size] = '\0';

    fclose(file);
    return 0;
}

int write_text(const char *filename, const char *line, const char *mode)
{
    assert(filename);
    assert(line);
    assert(mode);

    FILE *file = fopen(filename, mode);
    if (!file)
        return errno;

    if (fprintf(file, "%s", line) < 0)
    {
        log("Cant write line %s to file %s", line, filename);
        fclose(file);
        return errno;
    }

    fclose(file);
    return 0;
}

int write_strings(const char *filename, struct String *strings, size_t length, const char *mode)
{
    for (size_t i = 0; i < length; i++)
    {
        flog("Writing string... i = %lu, s = <%.*s>", i, (int)strings[i].length, strings[i].p);
        int error = write_string(filename, strings + i, mode);

        if (error)
            return error;
    }
    return 0;
}

int write_string(const char *filename, struct String *str, const char *mode)
{
    assert(filename);
    assert(str);
    assert(mode);

    FILE *file = fopen(filename, mode);
    if (!file)
        return errno;

    size_t written_count = fwrite(str->p, sizeof(char), str->length, file);

    // проверка чтобы записанная строка гарантировано перевела строку
    if (str->p[str->length - 1] != '\n')
    {
        if (fputc('\n', file) == EOF)
        {
            int error = errno;
            log("Error while writing char '\\n' in file %s (%d)", filename, error);
            fclose(file);
            return error;
        }
    }

    if (written_count < str->length)
    {
        int error = errno;
        log("Error while writing line <%.*s> in file %s (%d)", (int)str->length, str->p, filename, error);
        fclose(file);
        return error;
    }

    fclose(file);
    return 0;
}

int write_bin(const char *filename, void *arr, size_t el_size, size_t arr_size)
{
    assert(filename);
    assert(arr);

    FILE *file = fopen(filename, "wb");
    if (file == NULL)
    {
        int error = errno;
        log("Cant open file %s: %d", filename, error);
        return errno;
    }

    size_t written = fwrite(arr, el_size, arr_size, file);
    if (written != arr_size)
    {
        int error = errno;
        log("Error while writing data in file %s: %d", filename, error);
        return error;
    }

    fclose(file);
    return 0;
}