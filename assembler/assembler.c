#include "assembler.h"
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include "file_wrapper.h"
#include "my_string.h"
#include "config.h"
#include "log.h"

static int process_string(char *buffer, struct String *string, size_t buffer_size);

/*
Данный интерпретатор никак не проверяет корректность переданных аргументов для каждой из команд,
он просто заменяет названия команд на их кодовые номера из asm_table.h. Проверка того,
чтобы команды были записаны корректно (с корректными аргуменами) лежит целиком и полностью
на VM
*/
int asm_data(struct FileWrapper *file, const char *output_file_name)
{
    assert(file);
    assert(output_file_name);

    FILE *output_file = fopen(output_file_name, "w");
    if (!output_file)
    {
        int error = errno;
        log("Can not open output file <%s>: %d", output_file_name, error);
        return error;
    }

    for (size_t i = 0; i < file->strings_count; i++)
    {
        char buffer[MAX_COMMAND_LENGTH] = {};

        int error = process_string(buffer, file->strings + i, sizeof(buffer));

        if (error)
        {
            fclose(output_file);
            log("Can not process command [%lu] of file %s", i, file->filename);
            remove(output_file_name);
            return error;
        }

        if (fputs(buffer, output_file) == EOF)
        {
            error = errno;
            fclose(output_file);
            log("Error while writing command string in file %s: %d", output_file_name, error);
            remove(output_file_name);
            return error;
        }
    }

    fclose(output_file);

    return 0;
}

static int process_string(char *buffer, struct String *string, size_t buffer_size)
{
    assert(buffer);
    assert(string);
    assert(buffer_size);

    // str - та же строка что и в string->p, но с \0 на конце
    char real_string[MAX_COMMAND_LENGTH] = {};

    char str_command[MAX_COMMAND_LENGTH] = {};
    int arg = 0;

    // записываем в real_string ровно столько символов, сколько лежат в string->length (+ \0)
    if (snprintf(real_string, sizeof(real_string), "%.*s", (int)string->length, string->p) < 0)
    {
        log("Error while copy String string to char *real_string");
        return -1;
    }

    int res = sscanf(real_string, "%s %d", str_command, &arg);

    if (res < 1)
    {
        log("Cant read command: input format is not corrected");
        return -1;
    }

    enum Commands command = get_command(str_command);
    if (command == UNKNOWN)
    {
        log("Got unknown command: %s", str_command);
        return 1;
    }

    if (res == 1)
    {
        if (snprintf(buffer, buffer_size, "%d\n", command) < 0)
        {
            log("Error while print formatted command string to buffer");
            return -1;
        }
    }
    else if (res == 2)
    {
        if (snprintf(buffer, buffer_size, "%d %d\n", command, arg) < 0)
        {
            log("Error while print formatted command string to buffer");
            return -1;
        }
    }

    return 0;
}

enum Commands get_command(char *command)
{
    for (size_t i = 0; i < N_COMMANDS; i++)
    {
        if (!strcmp(command, commands[i]))
            return (enum Commands)i;
    }

    log("Got unknown command: %s", command);

    return UNKNOWN;
}