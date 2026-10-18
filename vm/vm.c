#include "vm.h"
#include <stdio.h>
#include <assert.h>
#include <sys/stat.h>
#include "executor.h"

static struct Command get_next_command(VM *vm);
static off_t get_file_size(FILE *file);
static inline size_t min(size_t a, size_t b);

VMError init_vm(VM *vm, vm_el_t *ram_p)
{
    assert(vm);
    assert(ram_p);

    VMError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK, .vm_error = VM_OK};

    vm->code_length = 0;
    vm->IP = 0;

    error.stack_error = init_stack(&vm->stack, START_STACK_CAPACITY);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in vm stack initialization: %s", get_stack_error(error.stack_error));
        return error;
    }

    error.stack_error = init_stack(&vm->ret_addresses_stack, START_STACK_CAPACITY);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in ret_addresses_stack initialization: %s", get_stack_error(error.stack_error));
        return error;
    }

    vm->registers_count = REGISTERS_COUNT;
    vm->RAM = ram_p;

    return error;
}

VMError load_program(VM *vm, const char *filename)
{
    assert(vm);
    assert(filename);

    VMError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK, .vm_error = VM_OK};

    FILE *file = fopen(filename, "rb");

    if (!file)
    {
        LOG("Cant open file %s", filename);
        error.vm_error = VM_CANT_OPEN_PROGRAM_FILE;
        return error;
    }

    off_t file_size = get_file_size(file);
    if (file_size < 0)
    {
        LOG("Cant get size of file %s", filename);
        fclose(file);
        error.vm_error = VM_CANT_GET_FILE_SIZE;
        return error;
    }

    size_t code_length = (size_t)file_size / sizeof(vm_el_t);

    size_t read_count = fread(vm->code, sizeof(vm_el_t), min(code_length, MAX_CODE_LENGTH), file);

    if (read_count != code_length)
    {
        LOG("Error while reading program file %s: read %lu commands but %lu expected", filename, read_count, code_length);
        fclose(file);
        error.vm_error = VM_ERROR_READING_FILE;
        return error;
    }

    vm->code_length = code_length;

    fclose(file);

    return error;
}

VMError execute_program(VM *vm)
{
    assert(vm);
    assert(vm->code);
    assert(vm->code_length);

    VMError vm_error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK, .vm_error = VM_OK};

    while (vm->IP < vm->code_length)
    {
        get_next_command(vm);

        ExecutorError error = execute_command(vm);
        if (error.executor_error == EXECUTOR_END_PROGRAM)
            break;

        if (error.stack_error || error.executor_error)
        {
            LOG("ERROR while executing command %lu", vm->IP);

            vm_error.stack_error = error.stack_error;
            vm_error.executor_error = error.executor_error;
            vm_error.vm_error = VM_ERROR_EXECUTE_COMMAND;
            break;
        }
    }
    return vm_error;
}

VMError babah_vm(VM *vm)
{
    assert(vm);

    VMError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK, .vm_error = VM_OK};

    if (is_stack_empty(&vm->stack) != STACK_OK)
    {
        error.stack_error = destroy_stack(&vm->stack);
        if (error.stack_error)
        {
            LOG("Error while destroing vm: %s", get_stack_error(error.stack_error));
            return error;
        }
    }

    if (is_stack_empty(&vm->ret_addresses_stack) != STACK_OK)
    {
        if (error.stack_error)
        {
            LOG("Error while destroing ret_addresses_stack: %s", get_stack_error(error.stack_error));
            return error;
        }
    }

    return error;
}

// ExecutorError execute_command(VM *vm);
#include "command_switch.h"

const char *get_vm_error(VirtualMachineError error)
{
    return STR_VM_ERRORS[error];
}

void print_vm_error(VMError error)
{
    LOG("ERROR HAPPENS:");
    LOG("Stack error: %s", get_stack_error(error.stack_error));
    LOG("Executor error: %s", get_exec_error(error.executor_error));
    LOG("VM error: %s", get_vm_error(error.vm_error));
}

static struct Command get_next_command(VM *vm)
{
    assert(vm);

    assert(vm->IP < vm->code_length);
    enum Commands cmd = (enum Commands)vm->code[vm->IP++];

    assert(cmd < N_COMMANDS);
    struct Command const_command = commands[cmd];
    assert(cmd == const_command.cmd);

    size_t args_count = const_command.cmd_args_count;

    struct Command final_command = {};

    switch (args_count)
    {
    case 0:
        final_command.cmd = cmd;
        break;
    case 1:
        final_command.cmd = cmd;
        final_command.cmd_args_count = 1;

        assert(vm->IP < vm->code_length);
        final_command.arg1 = vm->code[vm->IP++];
        break;
    case 2:
        final_command.cmd = cmd;
        final_command.cmd_args_count = 2;

        assert(vm->IP < vm->code_length);
        final_command.arg1 = vm->code[vm->IP++];

        assert(vm->IP < vm->code_length);
        final_command.arg2 = vm->code[vm->IP++];
        break;
    default:
        LOG("ERROR: COMMAND FROM CONFIG HAS %lu ARGUMENTS!!!", args_count);
    }
    vm->current_command = final_command;
    return final_command;
}

// < 0 если ошибка, иначе > 0
static off_t get_file_size(FILE *file)
{
    assert(file);

    struct stat st = {};

    // произошла ошибка
    if (fstat(fileno(file), &st))
    {
        LOG("Cant get file size");
        return -1;
    }

    return st.st_size;
}

static inline size_t min(size_t a, size_t b)
{
    return a < b ? a : b;
}