#include "assembler.h"
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include "file_wrapper/file_wrapper.h"
#include "file_wrapper/my_string.h"
#include "file_wrapper/files.h"
#include "config.h"
#include "log.h"

static enum ProcessStringError process_string(struct String *string, struct IR *ir);
static struct Command get_command(char *command);
static int write_ir_to_buffer(char *buffer, size_t buffer_size, struct IR *ir);

int asm_data(struct FileWrapper *file, const char *output_file_name, const char *output_debug_file_name)
{
    assert(file);
    assert(output_file_name);
    assert(output_debug_file_name);

    FILE *output_file = fopen(output_debug_file_name, "w");
    if (!output_file)
    {
        int error = errno;
        log("Can not open output file <%s>: %d", output_debug_file_name, error);
        return error;
    }

    long long code[MAX_CODE_LENGTH] = {};
    size_t code_index = 0;

    for (size_t i = 0; i < file->strings_count; i++)
    {
        struct IR ir = {};
        char buffer[MAX_COMMAND_LENGTH] = {};

        enum ProcessStringError error = process_string(file->strings + i, &ir);

        if (error == PSE_EMPTY_STRING)
            continue;

        if (error != PSE_OK)
        {
            fclose(output_file);
            log("Can not process command at line %lu of file %s", i + 1, file->filename);
            remove(output_debug_file_name);
            return error;
        }

        if (write_ir_to_buffer(buffer, sizeof(buffer), &ir))
        {
            log("Error while write formatted command string to buffer");
            return -1;
        }

        if (fputs(buffer, output_file) == EOF)
        {
            int _error = errno;
            fclose(output_file);
            log("Error while writing command string in file %s: %d", output_debug_file_name, _error);
            remove(output_debug_file_name);
            return _error;
        }

        code[code_index++] = ir.cmd;

        if (ir.cmd_args_count > 0)
            code[code_index++] = ir.arg1;

        if (ir.cmd_args_count > 1)
            code[code_index++] = ir.arg2;
    }

    fclose(output_file);

    int error = write_bin(output_file_name, (void *)code, sizeof(code[-2]), code_index);
    if (error)
    {
        log("Error while writing code to bin file %s: %d", output_file_name, error);
        return error;
    }

    return 0;
}

static enum ProcessStringError process_string(struct String *string, struct IR *ir)
{
    assert(string);
    assert(ir);

    // real_string - та же строка что и в string->p, но с \0 на конце
    char real_string[MAX_COMMAND_LENGTH] = {};

    char str_command[MAX_COMMAND_LENGTH] = {};
    int arg1 = 0, arg2 = 0;

    // записываем в real_string ровно столько символов, сколько лежат в string->length (+ \0)
    if (snprintf(real_string, sizeof(real_string), "%.*s", (int)string->length, string->p) < 0)
    {
        log("Error while copy String string to char *real_string");
        return PSE_COPY_ERROR;
    }

    if (is_space_string(real_string))
    {
        return PSE_EMPTY_STRING;
    }

    int res = sscanf(real_string, "%s %d %d", str_command, &arg1, &arg2);

    if (res < 1)
    {
        log("Cant read command: input format is not corrected");
        return PSE_INCORRECT_INPUT;
    }

    struct Command command = get_command(str_command);
    if (command.cmd_num == UNKNOWN)
    {
        log("Got unknown command: %s", str_command);
        return PSE_UNKNOWN_COMMAND;
    }

    if (res - 1 != (int)command.cmd_args_count)
    {
        log("%s: invalid arguments count: waited %lu, got %d", command.cmd_str, command.cmd_num, res - 1);
        return PSE_INVALID_ARGS;
    }

    ir->cmd = command.cmd_num;
    ir->cmd_args_count = command.cmd_args_count;
    ir->arg1 = arg1;
    ir->arg2 = arg2;

    return PSE_OK;
}

static struct Command get_command(char *command)
{
    assert(command);

    for (size_t i = 0; i < N_COMMANDS; i++)
    {
        struct Command cmd = commands[i];

        if (!strcmp(command, cmd.cmd_str))
            return cmd;
    }

    log("Got unknown command: %s", command);

    return (struct Command){.cmd_num = UNKNOWN, .cmd_str = "", .cmd_args_count = 0};
}

// 1 если все плохо, 0 если все хорошо
static int write_ir_to_buffer(char *buffer, size_t buffer_size, struct IR *ir)
{
    assert(buffer);
    assert(ir);

    switch (ir->cmd_args_count)
    {
    case 0:
        return snprintf(buffer, buffer_size, "%d\n", ir->cmd) < 0;
    case 1:
        return snprintf(buffer, buffer_size, "%d %d\n", ir->cmd, ir->arg1) < 0;
    case 2:
        return snprintf(buffer, buffer_size, "%d %d %d\n", ir->cmd, ir->arg1, ir->arg2) < 0;
    default:
        return 1;
    }
}