#include "assembler.h"
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include "file_wrapper/file_wrapper.h"
#include "file_wrapper/my_string.h"
#include "file_wrapper/files.h"
#include "config.h"
#include "log.h"

static enum ProcessStringError process_labels(struct ASM *asm_obj);
static enum ProcessStringError process_string(struct String *string, struct IR *ir, struct ASM *asm_obj);
static struct Command get_command(const char *command, const char *str_arg1, const char *str_arg2);
int get_arg_type(const char *str_arg, enum ArgTypes *arg_type_buf);
static int parse_command_args(struct ASM *asm_obj, struct Command *command, const char *str_arg1, const char *str_arg2, int *arg1, int *arg2);
static int parse_command_arg(struct ASM *asm_obj, enum ArgTypes arg_type, const char *str_arg, int *arg);
static int get_register(const char *str_register);
static int get_label(struct ASM *asm_obj, const char *label, struct Label *label_buf);
static inline void format_label_string(struct String *string);
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

    struct ASM asm_obj = {.file = file};

    enum ProcessStringError labels_error = process_labels(&asm_obj);
    if (labels_error != PSE_OK)
    {
        log("Error while processing labels");
        return labels_error;
    }

    for (size_t i = 0; i < file->strings_count; i++)
    {
        struct IR ir = {};
        char buffer[MAX_COMMAND_LENGTH] = {};

        enum ProcessStringError error = process_string(file->strings + i, &ir, &asm_obj);

        if (error == PSE_EMPTY_STRING || error == PSE_IS_LABEL)
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

        assert(asm_obj.code_index < MAX_CODE_LENGTH);
        asm_obj.code[asm_obj.code_index++] = ir.cmd;

        if (ir.cmd_args_count > 0)
        {
            assert(asm_obj.code_index < MAX_CODE_LENGTH);
            asm_obj.code[asm_obj.code_index++] = ir.arg1;
        }

        if (ir.cmd_args_count > 1)
        {
            assert(asm_obj.code_index < MAX_CODE_LENGTH);
            asm_obj.code[asm_obj.code_index++] = ir.arg2;
        }
    }

    fclose(output_file);

    int error = write_bin(output_file_name, (void *)asm_obj.code, sizeof(asm_obj.code[0]), asm_obj.code_index);
    if (error)
    {
        log("Error while writing code to bin file %s: %d", output_file_name, error);
        return error;
    }

    return 0;
}

static enum ProcessStringError process_labels(struct ASM *asm_obj)
{
    assert(asm_obj);

    asm_obj->labels_count = 0;
    size_t label_pos = 0;

    for (size_t i = 0; i < asm_obj->file->strings_count; i++)
    {
        struct String string = asm_obj->file->strings[i];
        struct IR ir = {};

        enum ProcessStringError error = process_string(&string, &ir, asm_obj);

        if (error != PSE_OK && error != PSE_IS_LABEL && error != PSE_EMPTY_STRING)
        {
            log("Error while labels processing in line %d", i + 1);
            return error;
        }

        if (error == PSE_OK)
        {
            label_pos++;
            label_pos += ir.cmd_args_count;
        }

        else if (error == PSE_IS_LABEL)
        {
            format_label_string(&string);

            assert(asm_obj->labels_count < MAX_LABELS_COUNT);
            asm_obj->labels[asm_obj->labels_count++] = (struct Label){
                .str = string,
                .position = label_pos,
            };
        }
    }
    asm_obj->labels_ready = 1;
    return PSE_OK;
}

static enum ProcessStringError process_string(struct String *string, struct IR *ir, struct ASM *asm_obj)
{
    assert(string);
    assert(ir);
    assert(asm_obj);

    // real_string - та же строка что и в string->p, но с \0 на конце
    char real_string[MAX_COMMAND_LENGTH] = {};

    char str_command[MAX_COMMAND_LENGTH] = {};
    char str_arg1[MAX_COMMAND_ARG_LENGTH] = {}, str_arg2[MAX_COMMAND_ARG_LENGTH] = {};
    int arg1 = 0, arg2 = 0;

    // записываем в real_string ровно столько символов, сколько лежат в string->length (+ \0)
    if (snprintf(real_string, sizeof(real_string), "%.*s", (int)string->length, string->p) < 0)
    {
        log("Error while copy String string to char *real_string");
        return PSE_COPY_ERROR;
    }

    char *comment_symb = strchr(real_string, COMMENT_CHAR);
    if (comment_symb != NULL)
    {
        *comment_symb = '\0';
    }

    if (is_space_string(real_string))
    {
        return PSE_EMPTY_STRING;
    }

    int res = sscanf(real_string, "%s %s %s", str_command, str_arg1, str_arg2);

    // является меткой
    if (str_command[0] == ':')
    {
        return PSE_IS_LABEL;
    }

    if (res < 1)
    {
        log("Cant read command: input format is not corrected");
        return PSE_INCORRECT_INPUT;
    }

    struct Command command = get_command(str_command, str_arg1, str_arg2);
    if (command.cmd_num == UNKNOWN)
    {
        log("Got unknown command: %s (%s, %s)", str_command, str_arg1, str_arg2);
        return PSE_UNKNOWN_COMMAND;
    }

    if (res - 1 != (int)command.cmd_args_count)
    {
        log("%s: invalid arguments count: waited %lu, got %d", command.cmd_str, command.cmd_args_count, res - 1);
        return PSE_INVALID_ARGS;
    }

    ir->cmd = command.cmd_num;
    ir->cmd_args_count = command.cmd_args_count;

    if (asm_obj->labels_ready)
    {
        if (parse_command_args(asm_obj, &command, str_arg1, str_arg2, &arg1, &arg2))
        {
            log("Error while parsing command args!");
            return PSE_INVALID_ARGS;
        }

        ir->arg1 = arg1;
        ir->arg2 = arg2;
    }

    return PSE_OK;
}

static struct Command get_command(const char *command, const char *str_arg1, const char *str_arg2)
{
    assert(command);
    assert(str_arg1);
    assert(str_arg2);

    enum ArgTypes arg1_type = ATYPE_VOID, arg2_type = ATYPE_VOID;

    if (get_arg_type(str_arg1, &arg1_type))
    {
        log("Get arg with unknown arg type: %s", str_arg1);
        return (struct Command){.cmd_num = UNKNOWN, .cmd_str = "", .cmd_args_count = 0};
    }
    if (get_arg_type(str_arg2, &arg2_type))
    {
        log("Get arg with unknown arg type: %s", str_arg2);
        return (struct Command){.cmd_num = UNKNOWN, .cmd_str = "", .cmd_args_count = 0};
    }

    for (size_t i = 0; i < N_COMMANDS; i++)
    {
        struct Command cmd = commands[i];

        if (!strcmp(command, cmd.cmd_str) && arg1_type == cmd.arg1_type && arg2_type == cmd.arg2_type)
            return cmd;
    }

    log("Got unknown command: %s", command);

    return (struct Command){.cmd_num = UNKNOWN, .cmd_str = "", .cmd_args_count = 0};
}

int get_arg_type(const char *str_arg, enum ArgTypes *arg_type_buf)
{
    assert(str_arg);
    assert(arg_type_buf);

    if (!str_arg[0])
    {
        *arg_type_buf = ATYPE_VOID;
        return 0;
    }

    if (str_arg[0] == ':')
    {
        *arg_type_buf = ATYPE_LABEL;
        return 0;
    }

    size_t length = strlen(str_arg);
    int is_memory = 0;
    char copy_str[MAX_COMMAND_ARG_LENGTH] = {};
    strncpy(copy_str, str_arg, MAX_COMMAND_ARG_LENGTH);
    char *copy_str_p = copy_str;

    if (str_arg[0] == '[' && str_arg[length - 1] == ']')
    {
        is_memory = 1;
        copy_str_p[length - 1] = '\0';
        copy_str_p++;
    }

    if (is_num(copy_str_p))
    {
        *arg_type_buf = is_memory ? ATYPE_MEMORY_INT : ATYPE_INT;
        return 0;
    }

    for (size_t i = 0; i < REGISTERS_COUNT; i++)
    {
        if (!strcmp(copy_str_p, REGISTERS[i]))
        {
            *arg_type_buf = is_memory ? ATYPE_MEMORY_REGISTER : ATYPE_REGISTER;
            return 0;
        }
    }
    log("Get argument with unknown arg type: %s", str_arg);
    return 1;
}

static int parse_command_args(struct ASM *asm_obj, struct Command *command, const char *str_arg1, const char *str_arg2, int *arg1, int *arg2)
{
    assert(command);
    assert(str_arg1);
    assert(str_arg2);
    assert(arg1);
    assert(arg2);

    if (command->cmd_args_count == 0)
        return 0;

    if (command->cmd_args_count > 0)
    {
        int res = parse_command_arg(asm_obj, command->arg1_type, str_arg1, arg1);
        if (res)
        {
            log("Error while parsing command %s arg1 %s", command->cmd_str, str_arg1);
            return res;
        }
    }
    if (command->cmd_args_count > 1)
    {
        int res = parse_command_arg(asm_obj, command->arg2_type, str_arg2, arg2);
        if (res)
        {
            log("Error while parsing command %s arg1 %s", command->cmd_str, str_arg2);
            return res;
        }
    }

    return 0;
}

static int parse_command_arg(struct ASM *asm_obj, enum ArgTypes arg_type, const char *str_arg, int *arg)
{
    switch (arg_type)
    {
    case ATYPE_VOID:
        break;

    case ATYPE_INT:
    {
        int res = sscanf(str_arg, "%d", arg);

        if (res == 1)
            break;

        log("Cant parse INT arg from str_arg = %s", str_arg);
        return 1;
    }

    case ATYPE_MEMORY_INT:
    {
        int res = sscanf(str_arg, "[%d]", arg);

        if (res == 1)
            break;

        log("Cant parse INT arg from str_arg = %s", str_arg);
        return 1;
    }

    case ATYPE_REGISTER:
    {
        int reg = get_register(str_arg);
        if (reg < 0)
        {
            log("Get unknown register name: %s", str_arg);
            return 1;
        }
        *arg = reg;
        break;
    }

    case ATYPE_MEMORY_REGISTER:
    {
        char copy_str[MAX_COMMAND_ARG_LENGTH] = {};
        strncpy(copy_str, str_arg, MAX_COMMAND_ARG_LENGTH);
        char *copy_str_p = copy_str + 1;
        copy_str_p[strlen(copy_str_p) - 1] = '\0';

        int reg = get_register(copy_str_p);
        if (reg < 0)
        {
            log("Get unknown register name: %s", str_arg);
            return 1;
        }
        *arg = reg;
        break;
    }

    case ATYPE_LABEL:
    {
        struct Label label = {};

        int res = get_label(asm_obj, str_arg, &label);
        if (res)
        {
            log("Error while getting label %s", str_arg);
            return 1;
        }
        *arg = (int)label.position;
        break;
    }

    default:
        log("Unknown arg type: %d", arg_type);
        return 1;
    }

    return 0;
}

static int get_register(const char *str_register)
{
    assert(str_register);

    for (size_t i = 0; i < REGISTERS_COUNT; i++)
    {
        if (!strcmp(str_register, REGISTERS[i]))
            return (int)i;
    }

    log("Got unknown register: %s", str_register);
    return -1;
}

static int get_label(struct ASM *asm_obj, const char *label, struct Label *label_buf)
{
    assert(asm_obj);
    assert(label);

    for (size_t i = 0; i < asm_obj->labels_count; i++)
    {
        struct Label lbl = asm_obj->labels[i];

        if (!string_strcmp(lbl.str, label))
        {
            *label_buf = lbl;
            return 0;
        }
    }
    log("Undefined label: %s", label);
    return 1;
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

static inline void format_label_string(struct String *string)
{
    assert(string);

    // убираем комментарии
    const char *comment_symb = string_chr(string, COMMENT_CHAR);
    if (comment_symb != NULL)
    {
        string->length = (size_t)(comment_symb - string->p);
    }

    // убираем лишние пробелы в начале и конце строки
    while (isspace(string->p[0]))
    {
        string->p++;
        string->length--;
    }
    while (isspace(string->p[string->length - 1]))
        string->length--;
}